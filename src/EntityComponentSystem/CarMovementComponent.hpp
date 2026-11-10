#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "Components.hpp"
#include "FrameManager.hpp"
#include "Game.hpp"
#include "Input.hpp"
#include "Map.hpp"
#include "Utils/Math.hpp"
#include "Utils/Physics.hpp"
#include "Utils/Polygon.hpp"
#include "Utils/Vector2D.hpp"

struct CarWheelConfig {
	Vector2D localPosition;
	Vector2D size;
	bool steerable;
	bool driven;

	CarWheelConfig(Vector2D localPosition, Vector2D size, bool steerable, bool driven) :
		localPosition(localPosition),
		size(size),
		steerable(steerable),
		driven(driven) {}

	CarWheelConfig(Vector2D size, bool steerable, bool driven) :
		size(size),
		steerable(steerable),
		driven(driven) {}

	CarWheelConfig(bool steerable, bool driven) :
		steerable(steerable),
		driven(driven) {}
};

class CarMovementComponent : public Component {
	public:
		TransformComponent * transform;
		
		Manager * manager = nullptr;
		std::vector<Entity*> wheelEntities;
		
		std::string wheelTexturePath;
		std::string wheelColliderTag = "wheel";
		Group group;

		VehicleControl input;
		Vector2D totalForce;
		float totalTorque = 0.0f;
		float airDrag = 0.485f;
		float drivetrainEfficiency = 0.88f;

		float wheelDirection;
		float turningSpeed;
		float wheelBase;

		float mass;
		float trackWidth;
		float cgHeight;
		float wheelRadius;
		float tireMu;

		float engineRPM;
		float idleRPM;
		float maxRPM;

		float engineThrottleGain;
		float engineSyncGain;
		float engineFriction;
		float wheelSyncGain;
		float freeWheelFollow;
		float brakeAngularDecel;
		float wheelAngularDrag;

		float maxSteerAngle;
		float groundDrag;
		float downforce;
		float yawDamping;
		float tireRelaxation;
		float lowSpeedLateralGripSpeed;
		float slipRatioDenom;
		float slipAngleDenom;

		Vector2D velocity;
		Vector2D acceleration;
		Vector2D previousAcceleration;
		float yawRate;

		TorqueCurve torqueCurve;
		GearBox gearbox;
		float shiftCooldown = 0.0f;
		float shiftDelay = 0.5f;
		float shiftClutchTimer = 0.0f;
		float shiftClutchDuration = 0.18f;
		float coastTime = 0.0f;
		float maxSubstep = 0.005f;
		PacejkaCurve longitudinalCurve;
		PacejkaCurve lateralCurve;
		std::vector<WheelPhysics> wheels;

		CarMovementComponent() = default;
		CarMovementComponent(
			Manager * manager,
			float turningSpeed,
			float mass,
			float wheelBase,
			float trackWidth,
			float cgHeight,
			float wheelRadius,
			float tireMu,
			float idleRPM,
			float maxRPM,
			float engineThrottleGain,
			float engineSyncGain,
			float engineFriction,
			float wheelSyncGain,
			float freeWheelFollow,
			float brakeAngularDecel,
			float wheelAngularDrag,
			float maxSteerAngle,
			float groundDrag,
			float downforce,
			float yawDamping,
			float tireRelaxation,
			float lowSpeedLateralGripSpeed,
			float slipRatioDenom,
			float slipAngleDenom,
			TorqueCurve torqueCurve,
			GearBox gearbox,
			PacejkaCurve longitudinalCurve,
			PacejkaCurve lateralCurve,
			const std::vector<CarWheelConfig>& wheels,
			const std::string& path,
			Group group
		) :
			transform(nullptr),
			manager(manager),
			wheelTexturePath(path),
			group(group),
			wheelDirection(0.0f),
			turningSpeed(turningSpeed),
			wheelBase(std::max(0.1f, wheelBase)),
			mass(std::max(1.0f, mass)),
			trackWidth(std::max(0.1f, trackWidth)),
			cgHeight(std::max(0.0f, cgHeight)),
			wheelRadius(std::max(0.01f, wheelRadius)),
			tireMu(std::max(0.0f, tireMu)),
			engineRPM(idleRPM),
			idleRPM(idleRPM),
			maxRPM(maxRPM),
			engineThrottleGain(engineThrottleGain),
			engineSyncGain(engineSyncGain),
			engineFriction(engineFriction),
			wheelSyncGain(wheelSyncGain),
			freeWheelFollow(freeWheelFollow),
			brakeAngularDecel(brakeAngularDecel),
			wheelAngularDrag(wheelAngularDrag),
			maxSteerAngle(maxSteerAngle),
			groundDrag(std::max(0.0f, groundDrag)),
			downforce(std::max(0.0f, downforce)),
			yawDamping(std::max(0.0f, yawDamping)),
			tireRelaxation(std::max(1.0f, tireRelaxation)),
			lowSpeedLateralGripSpeed(lowSpeedLateralGripSpeed),
			slipRatioDenom(slipRatioDenom),
			slipAngleDenom(slipAngleDenom),
			velocity(Vector2D()),
			acceleration(Vector2D()),
			previousAcceleration(Vector2D()),
			yawRate(0.0f),
			torqueCurve(std::move(torqueCurve)),
			gearbox(std::move(gearbox)),
			longitudinalCurve(std::move(longitudinalCurve)),
			lateralCurve(std::move(lateralCurve)) {
			setWheels(wheels);
		}

		void init() override {
			transform = &entity->getComponent<TransformComponent>();
			createWheelEntities();
		}

		void update() override {
			float frameDt = FrameManager::getDeltaTime();
			if (frameDt <= 0.0f)
				return;

			input = getInput();
			int steps = std::max(1, static_cast<int>(std::ceil(frameDt / maxSubstep)));
			float dt = frameDt / steps;

			for (int i = 0; i < steps; i++)
				step(dt, input);

			updateWheelEntities();
		}

		float getSpeed() const {
			return velocity.getModule();
		}

	private:
		float wheelBaseMeters() const {
			return std::max(0.1f, wheelBase / PIXELS_PER_METER);
		}

		float trackWidthMeters() const {
			return std::max(0.1f, trackWidth / PIXELS_PER_METER);
		}

		float cgHeightMeters() const {
			return std::max(0.0f, cgHeight / PIXELS_PER_METER);
		}

		float wheelRadiusMeters() const {
			return std::max(0.05f, wheelRadius / PIXELS_PER_METER);
		}

		float slipRatioDenomMeters() const {
			return std::max(0.1f, slipRatioDenom / PIXELS_PER_METER);
		}

		float slipAngleDenomMeters() const {
			return std::max(0.1f, slipAngleDenom / PIXELS_PER_METER);
		}

		float cgLongitudinalOffsetPixels() const {
			if (wheels.size() < 2)
				return 0.0f;

			float frontXSum = 0.0f, rearXSum = 0.0f;
			int frontCount = 0, rearCount = 0;
			bool frontDriven = false, rearDriven = false;
			for (size_t i = 0; i < wheels.size(); i++) {
				const WheelPhysics& w = wheels[i];
				bool isFront = (w.localPosition == Vector2D()) ? (i < wheels.size() / 2) : (w.localPosition.x >= 0.0f);
				float wx = (w.localPosition == Vector2D()) ? ((isFront ? 0.5f : -0.5f) * wheelBase) : w.localPosition.x;
				if (isFront) {
					frontXSum += wx;
					frontCount++;
					if (w.driven) frontDriven = true;
				} else {
					rearXSum += -wx;
					rearCount++;
					if (w.driven) rearDriven = true;
				}
			}
			if (frontCount == 0 || rearCount == 0)
				return 0.0f;

			float frontDist = frontXSum / frontCount;
			float rearDist = rearXSum / rearCount;
			if (std::abs(frontDist - rearDist) > 0.5f)
				return 0.0f;

			float span = frontDist + rearDist;
			if (frontDriven && !rearDriven)
				return 0.09f * span; // Transverse FWD: ~59% front / 41% rear static weight
			if (!frontDriven && rearDriven)
				return 0.03f * span; // Front-engine RWD: ~53% front / 47% rear static weight
			return 0.05f * span;     // AWD: ~55% front / 45% rear static weight
		}

		float yawInertia() const {
			float bodyLength = wheelBaseMeters();
			float bodyWidth = trackWidthMeters();
			if (transform) {
				float spriteLength = (transform->width * transform->getScale()) / PIXELS_PER_METER;
				float spriteWidth = (transform->height * transform->getScale()) / PIXELS_PER_METER;
				bodyLength = std::max(bodyLength, spriteLength);
				bodyWidth = std::max(bodyWidth, spriteWidth);
			}
			return mass * (bodyLength * bodyLength + bodyWidth * bodyWidth) / 12.0f;
		}

		void step(float dt, const VehicleControl& stepInput) {
			updateSteering(dt, stepInput.steer);
			updateTransmission(dt, stepInput);
			float totalWeight = updateNormalLoads();
			float axleTorque = updateEngineTorque(dt, stepInput);

			Vector2D tireForces;
			float yawTorque = 0.0f;
			addTireForces(dt, stepInput, totalWeight, axleTorque, tireForces, yawTorque);

			// Quadratic aerodynamic drag: F_drag = -C_drag * v * |v|
			float speed = getSpeed();
			Vector2D airResistance = velocity * (-airDrag * speed);
			Vector2D netForce = tireForces + airResistance;

			// Real Coulomb rolling resistance: F_rr = C_rr * mass * g opposing motion (impulse-clamped at v -> 0)
			Vector2D predictedVel = velocity + (netForce / mass) * dt;
			float rollingForceMag = groundDrag * mass * GRAVITY_SI;
			float rollingDeltaV = (rollingForceMag / mass) * dt;
			Vector2D rollingFriction = (predictedVel.getModule() > rollingDeltaV)
				? predictedVel.getDirection() * (-rollingForceMag)
				: (predictedVel * (-mass / dt));

			totalForce = netForce + rollingFriction;
			totalTorque = yawTorque;

			acceleration = totalForce / mass;
			velocity += acceleration * dt;
			if (velocity.getModule() < 1e-5f)
				velocity.zero();

			float angularAcceleration = totalTorque / yawInertia();
			yawRate += angularAcceleration * dt;
			yawRate -= yawRate * (yawDamping * 0.1f) * dt;
			if (getSpeed() < 0.05f && stepInput.throttle <= 0.0f)
				yawRate = moveToward(yawRate, 0.0f, yawDamping * 2.0f * dt);

			transform->position += velocity * (PIXELS_PER_METER * dt);
			transform->direction = clockLimit(transform->direction + yawRate * toDeg * dt, 0.0f, 360.0f);
			transform->velocity = velocity * PIXELS_PER_METER;
			transform->angularVelocity = yawRate * toDeg;

			previousAcceleration = acceleration;
		}

		VehicleControl getInput() {
			VehicleControl ctrl;

			if (transform->turnIntent == TurnDirection::LEFT)
				ctrl.steer = -1.0f;
			else if (transform->turnIntent == TurnDirection::RIGHT)
				ctrl.steer = 1.0f;
			else
				ctrl.steer = 0.0f;

			if (transform->moveIntent == MovementDirection::STILL)
				return ctrl;

			float forwardSpeed = velocity.dot(forward());
			float speed = getSpeed();

			if (transform->moveIntent == MovementDirection::BRAKE) {
				ctrl.brake = 1.0f;
			} else if (transform->moveIntent == MovementDirection::FORWARD) {
				if (forwardSpeed < -0.25f)
					ctrl.brake = 1.0f;
				else
					ctrl.throttle = 1.0f;
			} else if (transform->moveIntent == MovementDirection::BACKWARD) {
				if (gearbox.gear == 0 || (speed < 0.5f && forwardSpeed <= 0.25f))
					ctrl.throttle = 1.0f;
				else
					ctrl.brake = 1.0f;
			}
			return ctrl;
		}

		void updateSteering(float dt, float steerInput) {
			float speed = getSpeed();
			float forwardSpeed = velocity.dot(forward());
			float lateralSpeed = velocity.dot(right());

			float highSpeedScale = 1.0f / (1.0f + 0.11f * std::max(0.0f, speed - 3.0f));
			float baseMaxSteer = maxSteerAngle * std::max(0.16f, highSpeedScale);
			float target = steerInput * baseMaxSteer;

			// Allow full opposite-lock steering angle up to maxSteerAngle when counter-steering a slide
			if (speed > 2.0f && forwardSpeed > 0.5f) {
				float sideslipDeg = std::atan2(lateralSpeed, std::abs(forwardSpeed)) * toDeg;
				if (steerInput * sideslipDeg > 0.0f) {
					float counterSteerTarget = sideslipDeg + steerInput * baseMaxSteer * 0.5f;
					target = (steerInput > 0.0f)
						? std::max(target, counterSteerTarget)
						: std::min(target, counterSteerTarget);
				}
			}
			target = clamp(target, -maxSteerAngle, maxSteerAngle);

			wheelDirection = moveToward(wheelDirection, target, turningSpeed * dt);
			wheelDirection = clamp(wheelDirection, -maxSteerAngle, maxSteerAngle);

			float left = 0.0f, right = 0.0f;
			if (std::abs(wheelDirection) > 0.0001f) {
				float wb = wheelBaseMeters();
				float tw = trackWidthMeters();
				float distanceToCurve = wb / tand(wheelDirection);
				left = atand(wb, distanceToCurve + tw * 0.5f);
				right = atand(wb, distanceToCurve - tw * 0.5f);
			}
			for (WheelPhysics& wheel : wheels) {
				if (!wheel.steerable) {
					wheel.steerAngle = 0.0f;
					continue;
				}
				if (wheel.localPosition.y < 0.0f)
					wheel.steerAngle = left;
				else
					wheel.steerAngle = right;
			}
		}

		float gearRatioAt(int gearIndex) const {
			if (gearIndex < 0 || gearIndex > gearbox.maxGear())
				return 0.0f;
			return gearbox.ratios[gearIndex] * gearbox.finalDrive;
		}

		float rpmAfterShift(int targetGear) const {
			float currentRatio = gearRatioAt(gearbox.gear);
			float targetRatio = gearRatioAt(targetGear);

			if (std::abs(currentRatio) <= 0.0001f)
				return engineRPM;

			if (std::abs(targetRatio) <= 0.0001f)
				return idleRPM;

			float rpm = engineRPM * std::abs(targetRatio / currentRatio);
			return clamp(rpm, idleRPM, maxRPM);
		}

		float wheelTorqueScoreForGear(int gearIndex, float rpm) const {
			float ratio = gearRatioAt(gearIndex);

			if (std::abs(ratio) <= 0.0001f)
				return 0.0f;

			return torqueCurve.getTorque(rpm) * std::abs(ratio);
		}

		void updateTransmission(float dt, const VehicleControl& stepInput) {
			if (shiftCooldown > 0.0f)
				shiftCooldown = std::max(0.0f, shiftCooldown - dt);
			if (shiftClutchTimer > 0.0f)
				shiftClutchTimer = std::max(0.0f, shiftClutchTimer - dt);

			if (stepInput.throttle <= 0.0f && stepInput.brake <= 0.0f && std::abs(stepInput.steer) < 0.1f)
				coastTime += dt;
			else
				coastTime = 0.0f;

			float forwardSpeed = velocity.dot(forward());
			float speed = getSpeed();

			if ((transform->moveIntent == MovementDirection::STILL || transform->moveIntent == MovementDirection::BRAKE) && speed < 0.15f) {
				gearbox.setGear(1);
				shiftClutchTimer = 0.0f;
				return;
			}

			if (transform->moveIntent == MovementDirection::BACKWARD && (gearbox.gear == 0 || (speed < 0.5f && forwardSpeed <= 0.25f))) {
				gearbox.setGear(0);
				shiftClutchTimer = 0.0f;
				return;
			}

			if (transform->moveIntent == MovementDirection::FORWARD && gearbox.gear < 2 && forwardSpeed >= -0.25f) {
				gearbox.setGear(2);
				shiftClutchTimer = 0.0f;
			}

			if (gearbox.gear < 2 || shiftCooldown > 0.0f)
				return;

			int currentGear = gearbox.gear;
			int nextGear = currentGear + 1;
			int previousGear = currentGear - 1;
			float wheelRPM = clamp(std::abs(radToRpm((forwardSpeed / wheelRadiusMeters()) * gearbox.totalRatio())), idleRPM, maxRPM);

			// Full-throttle upshift right past peak power (require wheelRPM sync so launch wheelspin doesn't false-shift)
			if (stepInput.throttle > 0.0f && nextGear <= gearbox.maxGear() && engineRPM >= maxRPM * 0.935f && wheelRPM >= maxRPM * 0.90f) {
				gearbox.shiftUp();
				shiftClutchTimer = shiftClutchDuration;
				shiftCooldown = shiftDelay;
				coastTime = 0.0f;
				return;
			}

			// Straight-line cruising upshift when lifting off throttle cleanly
			if (coastTime >= 0.25f && nextGear <= gearbox.maxGear() && engineRPM >= idleRPM + (maxRPM - idleRPM) * 0.50f) {
				float nextRPM = rpmAfterShift(nextGear);
				if (nextRPM >= idleRPM + (maxRPM - idleRPM) * 0.34f) {
					gearbox.shiftUp();
					shiftClutchTimer = shiftClutchDuration;
					shiftCooldown = shiftDelay;
					coastTime = 0.0f;
					return;
				}
			}

			float downshiftThreshold = idleRPM + (maxRPM - idleRPM) * ((stepInput.throttle > 0.0f) ? 0.36f : 0.31f);
			if (previousGear >= 2 && std::min(engineRPM, wheelRPM) <= downshiftThreshold) {
				float previousRPM = clamp(std::min(engineRPM, wheelRPM) * std::abs(gearRatioAt(previousGear) / gearRatioAt(currentGear)), idleRPM, maxRPM);
				if (previousRPM <= maxRPM * 0.82f) {
					gearbox.shiftDown();
					shiftClutchTimer = shiftClutchDuration * 0.75f;
					shiftCooldown = shiftDelay;
					coastTime = 0.0f;
					return;
				}
			}
		}

		float updateNormalLoads() {
			float speed = getSpeed();
			float totalWeight = mass * GRAVITY_SI + downforce * speed * speed;
			if (wheels.empty())
				return totalWeight;

			Vector2D f = forward();
			Vector2D r = right();
			float longAcc = clamp(previousAcceleration.dot(f), -1.5f * GRAVITY_SI, 1.5f * GRAVITY_SI);
			float latAcc = clamp(previousAcceleration.dot(r), -1.5f * GRAVITY_SI, 1.5f * GRAVITY_SI);

			float wb = wheelBaseMeters();
			float tw = trackWidthMeters();
			float h = cgHeightMeters();
			float cgX = cgLongitudinalOffsetPixels();

			float frontDistSum = 0.0f, rearDistSum = 0.0f;
			int frontCount = 0, rearCount = 0;
			for (size_t i = 0; i < wheels.size(); i++) {
				const WheelPhysics& wheel = wheels[i];
				bool isFront = (wheel.localPosition == Vector2D()) ? (i < wheels.size() / 2) : (wheel.localPosition.x >= 0.0f);
				float rawX = (wheel.localPosition == Vector2D()) ? ((isFront ? 0.5f : -0.5f) * wheelBase) : wheel.localPosition.x;
				float xm = (rawX - cgX) / PIXELS_PER_METER;
				if (xm >= 0.0f) {
					frontDistSum += xm;
					frontCount++;
				} else {
					rearDistSum += -xm;
					rearCount++;
				}
			}

			float b = (frontCount > 0) ? (frontDistSum / frontCount) : (wb * 0.5f);
			float c = (rearCount > 0) ? (rearDistSum / rearCount) : (wb * 0.5f);
			float L = std::max(0.1f, b + c);

			// Marco Monster Eq. 5.1: static balance + longitudinal pitch load transfer
			float frontAxleLoad = (c / L) * totalWeight - (h / L) * mass * longAcc;
			float rearAxleLoad = (b / L) * totalWeight + (h / L) * mass * longAcc;
			// Lateral roll load transfer across track width
			float latTransfer = (h / tw) * mass * latAcc;

			int nFront = std::max(1, frontCount);
			int nRear = std::max(1, rearCount);

			for (size_t i = 0; i < wheels.size(); i++) {
				WheelPhysics& wheel = wheels[i];
				bool isFront = (wheel.localPosition == Vector2D()) ? (i < wheels.size() / 2) : (wheel.localPosition.x >= 0.0f);
				bool isLeft = (wheel.localPosition == Vector2D()) ? (i % 2 == 0) : (wheel.localPosition.y < 0.0f);
				float load = isFront ? (frontAxleLoad / nFront) : (rearAxleLoad / nRear);
				float rollShare = isFront ? 0.56f : 0.44f;
				load += isLeft ? (rollShare * latTransfer) : (-rollShare * latTransfer);
				wheel.normalLoad = clamp(load, totalWeight * 0.05f, totalWeight * 0.65f);
			}
			return totalWeight;
		}

		float updateEngineTorque(float dt, const VehicleControl& stepInput) {
			int drivenCount = 0;
			float drivenOmegaSum = 0.0f;
			for (const WheelPhysics& wheel : wheels) {
				if (wheel.driven) {
					drivenOmegaSum += wheel.omega;
					drivenCount++;
				}
			}

			if (gearbox.inNeutral() || drivenCount == 0) {
				float torqueNorm = torqueCurve.getTorque(engineRPM) / std::max(1.0f, torqueCurve.maxTorque);
				float dRPM = stepInput.throttle * torqueNorm * (maxRPM - idleRPM) * engineThrottleGain
						   - (engineRPM - idleRPM) * engineFriction * 2.5f;
				engineRPM = clamp(engineRPM + dRPM * dt, idleRPM, maxRPM);
				return 0.0f;
			}

			float avgDrivenOmega = drivenOmegaSum / drivenCount;
			float ratio = gearbox.totalRatio();
			float wheelCoupledRPM = radToRpm(avgDrivenOmega * ratio);
			float coupledRPM = std::max(idleRPM, wheelCoupledRPM);

			// Manual clutch disengagement window during gear shifts
			if (shiftClutchTimer > 0.0f) {
				engineRPM = clamp(moveToward(engineRPM, coupledRPM, (maxRPM - idleRPM) * 2.4f * dt), idleRPM, maxRPM);
				return 0.0f;
			}

			// Automatic clutch slip at launch/standstill; tight synchronization once coupled above launch RPM
			float launchRPM = idleRPM + stepInput.throttle * (maxRPM - idleRPM) * 0.28f;
			float targetRPM = std::max(coupledRPM, launchRPM);
			float syncRate = (coupledRPM >= launchRPM) ? (engineSyncGain * 8.0f) : (engineThrottleGain * 2.5f);
			engineRPM = clamp(engineRPM + (targetRPM - engineRPM) * clamp(syncRate * dt, 0.0f, 1.0f), idleRPM, maxRPM);

			// Engine torque curve + soft rev limiter + off-throttle engine braking
			float revLimiter = clamp((maxRPM - engineRPM) / std::max(1.0f, maxRPM * 0.02f), 0.0f, 1.0f);
			float throttleTorque = stepInput.throttle * revLimiter * torqueCurve.getTorque(engineRPM);
			float engBrakeFactor = clamp((wheelCoupledRPM - idleRPM) / std::max(1.0f, maxRPM - idleRPM), 0.0f, 1.0f);
			float engineBrakeTorque = (1.0f - stepInput.throttle) * engineFriction * torqueCurve.minTorque * (0.20f + 0.65f * engBrakeFactor);

			float netEngineTorque = throttleTorque - engineBrakeTorque;
			return netEngineTorque * ratio * drivetrainEfficiency;
		}

		Vector2D forward() const {
			return Vector2D::fromPolar(1.0f, transform->direction);
		}

		Vector2D right() const {
			Vector2D f = forward();
			return f.perpendicular();
		}

		Vector2D localToWorldMeters(Vector2D localPixels) const {
			return (forward() * localPixels.x + right() * localPixels.y) / PIXELS_PER_METER;
		}

		Vector2D wheelForward(const WheelPhysics& wheel) const {
			return Vector2D::fromPolar(1.0f, transform->direction + wheel.steerAngle);
		}

		Vector2D pointVelocity(Vector2D offsetMeters) const {
			Vector2D tangent = offsetMeters.perpendicular();
			return velocity + tangent * yawRate;
		}

		void addTireForces(
			float dt,
			const VehicleControl& stepInput,
			float totalWeight,
			float axleTorque,
			Vector2D& tireForcesOut,
			float& yawTorqueOut
		) {
			if (wheels.empty())
				return;

			float Rw = wheelRadiusMeters();
			float relaxation = clamp(tireRelaxation * dt, 0.0f, 1.0f);
			float srDenomMin = slipRatioDenomMeters();
			float saDenomMin = slipAngleDenomMeters();

			int drivenCount = 0;
			float drivenOmegaSum = 0.0f;
			float drivenLoadSum = 0.0f;
			for (const WheelPhysics& wheel : wheels) {
				if (wheel.driven) {
					drivenOmegaSum += wheel.omega;
					drivenLoadSum += wheel.normalLoad;
					drivenCount++;
				}
			}
			float avgDrivenOmega = (drivenCount > 0) ? (drivenOmegaSum / drivenCount) : 0.0f;

			// Wheel rotational inertia I = 1/2 * m_wheel * R^2 + reflected engine inertia in gear
			float baseWheelInertia = 0.5f * (0.03f * mass) * Rw * Rw;
			float ratio = gearbox.totalRatio();
			float reflectedEngineInertia = (!gearbox.inNeutral() && drivenCount > 0)
				? (0.08f * ratio * ratio / drivenCount)
				: 0.0f;

			float cgX = cgLongitudinalOffsetPixels();
			for (size_t i = 0; i < wheels.size(); i++) {
				WheelPhysics& wheel = wheels[i];
				bool isFront = (wheel.localPosition == Vector2D()) ? (i < wheels.size() / 2) : (wheel.localPosition.x >= 0.0f);
				bool isLeft = (wheel.localPosition == Vector2D()) ? (i % 2 == 0) : (wheel.localPosition.y < 0.0f);
				Vector2D rawPos = (wheel.localPosition == Vector2D())
					? Vector2D((isFront ? 0.5f : -0.5f) * wheelBase, (isLeft ? -0.5f : 0.5f) * trackWidth)
					: wheel.localPosition;
				Vector2D offsetM = localToWorldMeters(rawPos - Vector2D(cgX, 0.0f));
				Vector2D wheelVelocity = pointVelocity(offsetM);
				Vector2D wf = wheelForward(wheel);
				Vector2D wr = wf.perpendicular();

				float vx = wheelVelocity.dot(wf);
				float vy = wheelVelocity.dot(wr);
				float maxGrip = tireMu * wheel.normalLoad;

				// Lateral slip angle alpha (radians)
				float targetSlipAngle = (wheelVelocity.getModule() < 0.02f && std::abs(wheel.omega * Rw) < 0.02f && stepInput.throttle <= 0.0f)
					? 0.0f
					: std::atan2(vy, std::max(std::abs(vx), saDenomMin));
				wheel.slipAngle += (targetSlipAngle - wheel.slipAngle) * relaxation;

				// Combined-slip reduction of longitudinal peak with increasing slip angle
				float longSlipAngleFactor = std::max(0.25f, std::cos(clamp(std::abs(wheel.slipAngle) * 1.6f, 0.0f, 1.3f)));

				// Semi-implicit wheel angular velocity integration (Marco Monster Method 2)
				float srDenom = std::max(std::abs(vx), srDenomMin);
				float currentSR = clamp((wheel.omega * Rw - vx) / srDenom, -2.0f, 2.0f);
				float currentFx = longitudinalCurve.evaluate(currentSR) * maxGrip * longSlipAngleFactor;

				float dCurve = std::max(
					0.0f,
					(longitudinalCurve.evaluate(currentSR + 0.005f) - longitudinalCurve.evaluate(currentSR - 0.005f)) / 0.01f
				);
				float kRoad = Rw * Rw * maxGrip * longSlipAngleFactor * (dCurve / srDenom);

				float driveTorque = 0.0f;
				float lsdCoupling = 0.0f;
				if (wheel.driven && !gearbox.inNeutral() && drivenCount > 0) {
					float lsdBias = clamp(wheelSyncGain / 15.0f, 0.0f, 1.0f);
					float equalShare = 1.0f / drivenCount;
					float loadShare = (drivenLoadSum > 1.0f) ? (wheel.normalLoad / drivenLoadSum) : equalShare;
					float torqueShare = lsdBias * (0.7f * loadShare + 0.3f * equalShare) + (1.0f - lsdBias) * equalShare;
					lsdCoupling = wheelSyncGain * 1.2f;
					driveTorque = axleTorque * torqueShare + lsdCoupling * (avgDrivenOmega - wheel.omega);
				}

				float freeRollingOmega = vx / Rw;
				if ((driveTorque - Rw * currentFx) * (wheel.omega - freeRollingOmega) < 0.0f) {
					float secantK = Rw * std::abs(currentFx) / std::max(std::abs(wheel.omega - freeRollingOmega), 0.05f);
					kRoad = std::max(kRoad, secantK);
				}

				float effectiveInertia = baseWheelInertia + (wheel.driven ? reflectedEngineInertia : 0.0f);
				float angularDrag = wheelAngularDrag * baseWheelInertia;
				float denom = effectiveInertia + (kRoad + angularDrag + lsdCoupling) * dt;

				float nextOmega = wheel.omega + ((driveTorque - Rw * currentFx - angularDrag * wheel.omega) * dt) / denom;

				// Passive brake rotor/drum torque opposing wheel rotation (front-biased EBD + ABS threshold modulation)
				if (stepInput.brake > 0.0f) {
					float brakeBias = wheel.steerable ? 1.40f : 0.60f;
					float maxBrakeTorque = stepInput.brake * brakeBias
						* (1.25f * tireMu * (mass * GRAVITY_SI / std::max<size_t>(1, wheels.size())) * Rw)
						* (brakeAngularDecel / 120.0f);
					float maxBrakeDeltaOmega = (maxBrakeTorque * dt) / denom;
					float absFloorOmega = 0.0f;
					if (vx > 2.0f)
						absFloorOmega = (vx * 0.85f) / Rw;
					else if (vx < -2.0f)
						absFloorOmega = (vx * 0.85f) / Rw;
					nextOmega = moveToward(nextOmega, absFloorOmega, maxBrakeDeltaOmega);
				}
				wheel.omega = nextOmega;

				// Longitudinal slip ratio SR & Pacejka Magic Formula evaluation
				wheel.slipRatio = clamp((wheel.omega * Rw - vx) / srDenom, -2.0f, 2.0f);

				float fx = longitudinalCurve.evaluate(wheel.slipRatio) * maxGrip * longSlipAngleFactor;
				float latSlipRatioFactor = 1.0f / std::sqrt(1.0f + (1.8f * wheel.slipRatio) * (1.8f * wheel.slipRatio));
				float fy = -lateralCurve.evaluate(wheel.slipAngle) * maxGrip * latSlipRatioFactor;

				// Impulse caps prevent low-speed numerical overshoot across zero velocity
				float massShare = mass * (wheel.normalLoad / std::max(1.0f, totalWeight));
				float fyCap = (massShare * std::abs(vy)) / dt;
				fy = clamp(fy, -fyCap, fyCap);

				if (stepInput.throttle <= 0.0f && std::abs(wheel.omega * Rw) < 0.02f && std::abs(vx) < 0.25f) {
					float fxCap = (massShare * std::abs(vx)) / dt;
					fx = clamp(fx, -fxCap, fxCap);
				}

				// Friction Circle constraint: sqrt(Fx^2 + Fy^2) <= mu * Fz
				float forceMag = std::sqrt(fx * fx + fy * fy);
				if (forceMag > maxGrip && forceMag > 0.0001f) {
					float scale = maxGrip / forceMag;
					fx *= scale;
					fy *= scale;
				}

				Vector2D tireForce = wf * fx + wr * fy;
				tireForcesOut += tireForce;
				yawTorqueOut += offsetM.cross(tireForce);
			}
		}

		void createWheelEntities() {
			Vector2D defaultWheelSize;
			if (!wheels.empty())
				defaultWheelSize = wheels[0].size;
			for (size_t i = 0; i < wheels.size(); i++) {
				WheelPhysics& wheel = wheels[i];

				float x = (i < wheels.size() / 2) ? wheelBase * 0.5f : -wheelBase * 0.5f;
				float y = (i % 2 == 0) ? -trackWidth * 0.5f : trackWidth * 0.5f;
				wheel.localPosition = (wheel.localPosition == Vector2D()) ? Vector2D(x, y) : wheel.localPosition;
				Vector2D local = wheel.localPosition;
				wheel.size = (wheel.size == Vector2D()) ? defaultWheelSize : wheel.size;
				Vector2D size = wheel.size;

				Entity& wheelEntity = manager->addEntity();
				wheelEntities.push_back(&wheelEntity);
				auto& wheelTransform = wheelEntity.addComponent<TransformComponent>(local.x, local.y, size.x, size.y);
				wheelTransform.entity->setFather(entity);
				wheelEntity.addComponent<SpriteComponent>(wheelTexturePath);
				wheelEntity.addComponent<ColliderComponent>(wheelColliderTag);
				wheelEntity.addGroup(group);
			}
		}

		void setWheels(const std::vector<CarWheelConfig>& wheelConfigs) {
			wheels.clear();
			wheels.reserve(wheelConfigs.size());

			bool frontDriven = false, rearDriven = false;
			for (size_t i = 0; i < wheelConfigs.size(); i++) {
				const CarWheelConfig& config = wheelConfigs[i];
				WheelPhysics wheel;
				wheel.localPosition = config.localPosition;
				wheel.size = config.size;
				wheel.steerable = config.steerable;
				wheel.driven = config.driven;
				wheel.steerAngle = 0.0f;
				wheel.omega = 0.0f;
				wheel.normalLoad = 0.0f;
				wheel.slipRatio = 0.0f;
				wheel.slipAngle = 0.0f;
				wheels.push_back(wheel);

				bool isFront = (config.localPosition == Vector2D()) ? (i < wheelConfigs.size() / 2) : (config.localPosition.x >= 0.0f);
				if (config.driven) {
					if (isFront) frontDriven = true;
					else rearDriven = true;
				}
			}
			// Transverse FWD transaxle (~88% efficiency) vs longitudinal RWD/AWD (~79% efficiency)
			drivetrainEfficiency = (frontDriven && !rearDriven) ? 0.88f : 0.79f;
		}

		void updateWheelEntities() {
			size_t count = std::min(wheels.size(), wheelEntities.size());
			for (size_t i = 0; i < count; i++) {
				auto& wheelTransform = wheelEntities[i]->getComponent<TransformComponent>();

				wheelTransform.position = wheels[i].localPosition;
				wheelTransform.direction = wheels[i].steerAngle;
				wheelTransform.velocity = transform->velocity;
				wheelTransform.angularVelocity = transform->angularVelocity;
			}
		}
};