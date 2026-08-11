#pragma once

#include "SDL2/SDL.h"

#include "Components.hpp"
#include "Map.hpp"
#include "Vector2D.hpp"
#include "Input.hpp"
#include "Math.hpp"
#include "Physics.hpp"
#include "Polygon.hpp"
#include "FrameManager.hpp"
#include "Game.hpp"

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <utility>
#include <string>

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
};

struct CarMovementConfig {
	float turningSpeed;

	float mass;
	float wheelBase;
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
	float wheelDirection;

	TorqueCurve torqueCurve;
	GearBox gearbox;
	PacejkaCurve longitudinalCurve;
	PacejkaCurve lateralCurve;
	std::vector<CarWheelConfig> wheels;

	CarMovementConfig(
		float turningSpeed,
		float mass,
		float wheelBase,
		float trackWidth,
		float cgHeight,
		float wheelRadius,
		float tireMu,
		float engineRPM,
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
		Vector2D velocity,
		Vector2D acceleration,
		Vector2D previousAcceleration,
		float yawRate,
		float wheelDirection,
		TorqueCurve torqueCurve,
		GearBox gearbox,
		PacejkaCurve longitudinalCurve,
		PacejkaCurve lateralCurve,
		std::vector<CarWheelConfig> wheels
	) :
		turningSpeed(turningSpeed),
		mass(mass),
		wheelBase(wheelBase),
		trackWidth(trackWidth),
		cgHeight(cgHeight),
		wheelRadius(wheelRadius),
		tireMu(tireMu),
		engineRPM(engineRPM),
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
		groundDrag(groundDrag),
		downforce(downforce),
		yawDamping(yawDamping),
		tireRelaxation(tireRelaxation),
		lowSpeedLateralGripSpeed(lowSpeedLateralGripSpeed),
		slipRatioDenom(slipRatioDenom),
		slipAngleDenom(slipAngleDenom),
		velocity(velocity),
		acceleration(acceleration),
		previousAcceleration(previousAcceleration),
		yawRate(yawRate),
		wheelDirection(wheelDirection),
		torqueCurve(std::move(torqueCurve)),
		gearbox(std::move(gearbox)),
		longitudinalCurve(std::move(longitudinalCurve)),
		lateralCurve(std::move(lateralCurve)),
		wheels(std::move(wheels)) {}
};

class CarMovementComponent : public Component {
	public:
		TransformComponent * transform;
		
		Manager * manager = nullptr;
		std::vector<Entity*> wheelEntities;
		
		std::string wheelTexturePath;
		std::string wheelColliderTag = "wheel";
		Group group;

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
		float shiftDelay = 1.5f;
		PacejkaCurve longitudinalCurve;
		PacejkaCurve lateralCurve;
		std::vector<WheelPhysics> wheels;

		CarMovementComponent(Manager * manager, const CarMovementConfig& config, std::string path, Group group) :
			transform(nullptr),
			group(group),
			manager(manager),
			wheelTexturePath(path),
			wheelDirection(config.wheelDirection),
			turningSpeed(config.turningSpeed),
			wheelBase(std::max(0.1f, config.wheelBase)),
			mass(std::max(1.0f, config.mass)),
			trackWidth(std::max(0.1f, config.trackWidth)),
			cgHeight(std::max(0.0f, config.cgHeight)),
			wheelRadius(std::max(0.01f, config.wheelRadius)),
			tireMu(std::max(0.0f, config.tireMu)),
			engineRPM(config.engineRPM),
			idleRPM(config.idleRPM),
			maxRPM(config.maxRPM),
			engineThrottleGain(config.engineThrottleGain),
			engineSyncGain(config.engineSyncGain),
			engineFriction(config.engineFriction),
			wheelSyncGain(config.wheelSyncGain),
			freeWheelFollow(config.freeWheelFollow),
			brakeAngularDecel(config.brakeAngularDecel),
			wheelAngularDrag(config.wheelAngularDrag),
			maxSteerAngle(config.maxSteerAngle),
			groundDrag(config.groundDrag),
			downforce(config.downforce),
			yawDamping(config.yawDamping),
			tireRelaxation(config.tireRelaxation),
			lowSpeedLateralGripSpeed(config.lowSpeedLateralGripSpeed),
			slipRatioDenom(config.slipRatioDenom),
			slipAngleDenom(config.slipAngleDenom),
			velocity(config.velocity),
			acceleration(config.acceleration),
			previousAcceleration(config.previousAcceleration),
			yawRate(config.yawRate),
			torqueCurve(config.torqueCurve),
			gearbox(config.gearbox),
			longitudinalCurve(config.longitudinalCurve),
			lateralCurve(config.lateralCurve) {
			setWheels(config.wheels);
		}

		void init() override {
			transform = &entity->getComponent<TransformComponent>();
			createWheelEntities();
		}

		void update() override {
			float dt = FrameManager::getDeltaTime();
			VehicleControl input = getInput();
			
			updateSteering(dt, input.steer);
			updateTransmission(dt, input);
			updateNormalLoads();
			updateEngineAndDrivenWheels(dt, input);
			updateFreeWheelsAndBrakes(dt, input);

			Vector2D totalForce;
			float totalTorque = 0.0f;

			addTireForces(dt, totalForce, totalTorque);

			acceleration = totalForce/mass;
			float angularAcceleration = totalTorque/yawInertia();

			velocity += acceleration * dt;
			
			Vector2D dir = Vector2D::fromPolar(1,transform->getDirection());
			float signedVelocity = velocity.dot(dir);
			if (signedVelocity > 0)
				velocity = Vector2D::max(velocity-dir*groundDrag*GRAVITY*dt,Vector2D(),dir);
			else
				velocity = Vector2D::min(velocity+dir*groundDrag*GRAVITY*dt,Vector2D(),dir);
				
			yawRate += angularAcceleration * dt;
			yawRate -= yawRate * (yawDamping + std::sqrt(velocity.dot(velocity)) * 0.08f) * dt;

			transform->position += velocity * dt;
			transform->direction = clockLimit(transform->direction + yawRate * toDeg * dt, 0.0f, 360.0f);
			transform->velocity = velocity;
			transform->angularVelocity = yawRate * toDeg;
			
			updateWheelEntities();
			previousAcceleration = acceleration;
		}

		float getSpeed() const {
			return velocity.getModule();
		}

	private:
		VehicleControl getInput() {
			VehicleControl input;
		
			float forwardSpeed = velocity.dot(forward());
		
			if (transform->turnIntent == TurnDirection::LEFT)
				input.steer = -1.0f;
			else if (transform->turnIntent == TurnDirection::RIGHT)
				input.steer = 1.0f;
		
			if (transform->moveIntent == MovementDirection::FORWARD) {
				if (forwardSpeed < 0.0f)
					input.brake = 1.0f;
				else
					input.throttle = 1.0f;
			}
			else if (transform->moveIntent == MovementDirection::BACKWARD) {
				if (forwardSpeed > 0.0f)
					input.brake = 1.0f;
				else
					input.throttle = 1.0f;
			}
			return input;
		}

		void updateSteering(float dt, float steerInput) {
			float target = steerInput*maxSteerAngle;
			wheelDirection = moveToward(wheelDirection,target,turningSpeed*dt);
			wheelDirection = clamp(wheelDirection,-maxSteerAngle,maxSteerAngle);

			float left = 0, right = 0;
			if (wheelDirection) {
				float distance_to_curve = wheelBase/tand(wheelDirection);
				left = atand(wheelBase,distance_to_curve+trackWidth*0.5f);
				right = atand(wheelBase,distance_to_curve-trackWidth*0.5f);
			}
			for (WheelPhysics& wheel : wheels) {
				if (!wheel.steerable)
					continue;
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

		void updateTransmission(float dt, const VehicleControl& input) {
			if (shiftCooldown > 0.0f)
				shiftCooldown -= dt;
		
			float forwardSpeed = velocity.dot(forward());
			float speed = getSpeed();
		
			if (transform->moveIntent == MovementDirection::STILL && speed == 0.0f) {
				gearbox.setGear(1);
				return;
			}
		
			if (transform->moveIntent == MovementDirection::BACKWARD) {
				if (forwardSpeed <= 0.0f)
					gearbox.setGear(0);
		
				return;
			}
		
			if (transform->moveIntent != MovementDirection::FORWARD)
				return;
		
			if (forwardSpeed < 0.0f)
				return;
		
			if (gearbox.gear < 2)
				gearbox.setGear(2);
		
			if (input.throttle <= 0.0f)
				return;
		
			if (shiftCooldown > 0.0f)
				return;
		
			int currentGear = gearbox.gear;
			int nextGear = currentGear + 1;
			int previousGear = currentGear - 1;
		
			float upshiftRPM = maxRPM * 0.88f;
			float downshiftRPM = maxRPM * 0.38f;
		
			if (nextGear <= gearbox.maxGear()) {
				float nextRPM = rpmAfterShift(nextGear);
		
				if (torqueCurve.getTorque(nextRPM) >= torqueCurve.getTorque(engineRPM)) {
					gearbox.shiftUp();
					engineRPM = nextRPM;
					shiftCooldown = shiftDelay;
					return;
				}
			}
		
			if (previousGear >= 2) {
				float previousRPM = rpmAfterShift(previousGear);
		
				if (torqueCurve.getTorque(previousRPM) >= torqueCurve.getTorque(engineRPM)) {
					gearbox.shiftDown();
					engineRPM = previousRPM;
					shiftCooldown = shiftDelay;
					return;
				}
			}
		}
		
		Vector2D forward() const {
			return Vector2D::fromPolar(1.0f, transform->direction);
		}

		Vector2D localToWorld(Vector2D v) const {
			return forward()*v.x+right()*v.y;
		}
		
		Vector2D right() const {
			Vector2D f = forward();
			return f.perpendicular();
		}

		Vector2D wheelForward(const WheelPhysics& wheel) const {
			return Vector2D::fromPolar(1.0f, transform->direction + wheel.steerAngle);
		}

		Vector2D pointVelocity(Vector2D offset) const {
			Vector2D tangent = offset.perpendicular();
			return velocity+tangent*yawRate;
		}

		float yawInertia() const {
			float wb = wheelBase;
			float tw = trackWidth;
		
			return mass*(wb*wb+tw*tw)/12.0f;
		}

		float weight() const {
			return mass * GRAVITY;
		}

		int frontWheelCount() const {
			int count = 0;

			for (const WheelPhysics& wheel : wheels)
				if (wheel.localPosition.x >= 0.0f)
					count++;

			return count;
		}

		int rearWheelCount() const {
			int count = 0;

			for (const WheelPhysics& wheel : wheels)
				if (wheel.localPosition.x < 0.0f)
					count++;

			return count;
		}

		int positiveSideWheelCount() const {
			int count = 0;

			for (const WheelPhysics& wheel : wheels)
				if (wheel.localPosition.y >= 0.0f)
					count++;

			return count;
		}

		int negativeSideWheelCount() const {
			int count = 0;

			for (const WheelPhysics& wheel : wheels)
				if (wheel.localPosition.y < 0.0f)
					count++;

			return count;
		}

		void updateNormalLoads() {
			if (wheels.empty())
				return;

			float speedSq = velocity.dot(velocity);
			float totalWeight = weight() + downforce * speedSq;

			int frontCount = frontWheelCount();
			int rearCount = rearWheelCount();
			int positiveSideCount = positiveSideWheelCount();
			int negativeSideCount = negativeSideWheelCount();

			float frontAxleLoad = totalWeight/2.0f;
			float rearAxleLoad = totalWeight/2.0f;

			float longAcceleration = previousAcceleration.dot(forward());
			float latAcceleration = previousAcceleration.dot(right());

			float longTransfer = mass*longAcceleration*cgHeight/wheelBase;
			float lateralTransfer = mass*latAcceleration*cgHeight/trackWidth;

			frontAxleLoad -= longTransfer;
			rearAxleLoad += longTransfer;

			for (WheelPhysics& wheel : wheels) {
				bool frontAxle = wheel.localPosition.x >= 0.0f;
				bool positiveSide = wheel.localPosition.y >= 0.0f;

				float load = totalWeight/static_cast<float>(wheels.size());

				if (frontCount > 0 && rearCount > 0) {
					if (frontAxle)
						load = frontAxleLoad/static_cast<float>(frontCount);
					else
						load = rearAxleLoad/static_cast<float>(rearCount);
				}

				if (positiveSideCount > 0 && negativeSideCount > 0) {
					if (positiveSide)
						load -= lateralTransfer / static_cast<float>(positiveSideCount);
					else
						load += lateralTransfer / static_cast<float>(negativeSideCount);
				}
				wheel.normalLoad = load;
			}
		}

		float averageDrivenWheelOmega() const {
			float sum = 0.0f;
			int count = 0;

			for (const WheelPhysics& wheel : wheels) {
				if (wheel.driven) {
					sum += wheel.omega;
					count++;
				}
			}

			if (count == 0)
				return 0.0f;
			return sum/static_cast<float>(count);
		}

		void updateEngineAndDrivenWheels(float dt, const VehicleControl& input) {
			float torque = torqueCurve.getTorque(engineRPM);
			float ratio = gearbox.totalRatio();
		
			float throttleTargetRPM = idleRPM+input.throttle*(maxRPM-idleRPM);
		
			if (gearbox.inNeutral()) {
				float dRPM = (throttleTargetRPM-engineRPM)*engineThrottleGain;
				dRPM -= (maxRPM-idleRPM)*engineFriction;
		
				engineRPM += dRPM * dt;
				engineRPM = clamp(engineRPM,idleRPM,maxRPM);
				return;
			}
		
			if (input.throttle <= 0.0f) {	
				engineRPM -= (maxRPM-idleRPM)*engineFriction*dt;
				engineRPM = clamp(engineRPM,idleRPM,maxRPM);
				return;
			}
		
			float drivenOmega = averageDrivenWheelOmega();
			float expectedEngineRPM = std::abs(radToRpm(drivenOmega*ratio));
		
			float throttlePull = (throttleTargetRPM-engineRPM)*engineThrottleGain;
			float clutchPull = (expectedEngineRPM-engineRPM)*engineSyncGain;
			float friction = (maxRPM-idleRPM)*engineFriction;
		
			engineRPM += (throttlePull+clutchPull-friction)*dt;
			engineRPM = clamp(engineRPM,idleRPM,maxRPM);
		
			float expectedWheelOmega = rpmToRad(engineRPM) / ratio;
			float torqueScale = torque/std::max(1.0f, torqueCurve.maxTorque);
			float clutchScale = 0.15f+0.85f*input.throttle;
		
			for (WheelPhysics& wheel : wheels) {
				if (!wheel.driven)
					continue;
		
				float dOmega = wheelSyncGain*torqueScale*clutchScale*(expectedWheelOmega-wheel.omega);
				wheel.omega += dOmega * dt;
			}
		}

		void updateFreeWheelsAndBrakes(float dt, const VehicleControl& input) {
			for (WheelPhysics& wheel : wheels) {
				Vector2D offset = localToWorld(wheel.localPosition);
				Vector2D wheelVelocity = pointVelocity(offset);
				Vector2D wf = wheelForward(wheel);
				float longitudinalSpeed = wheelVelocity.dot(wf);
				float freeRollingOmega = longitudinalSpeed / wheelRadius;

				bool freeRolling = !wheel.driven || gearbox.inNeutral() || input.throttle <= 0.0f;

				if (freeRolling)
					wheel.omega += (freeRollingOmega - wheel.omega) * freeWheelFollow * dt;

				wheel.omega = moveToward(wheel.omega, 0.0f, brakeAngularDecel * input.brake * dt);
				wheel.omega -= wheel.omega * wheelAngularDrag * dt;
			}
		}

		void addTireForces(float dt, Vector2D& totalForce, float& totalTorque) {
			float relaxation = clamp(tireRelaxation * dt, 0.0f, 1.0f);

			for (WheelPhysics& wheel : wheels) {
				Vector2D offset = localToWorld(wheel.localPosition);
				Vector2D wheelVelocity = pointVelocity(offset);
				Vector2D wf = wheelForward(wheel);
				Vector2D wr = wf.perpendicular();

				float vx = wheelVelocity.dot(wf);
				float vy = wheelVelocity.dot(wr);
				float wheelSpeed = wheelVelocity.getModule();
				float surfaceSpeed = wheel.omega * wheelRadius;

				if (std::abs(surfaceSpeed) < 2.5f && wheelSpeed < 2.5f) {
					wheel.slipRatio = 0.0f;
					wheel.slipAngle = 0.0f;
					continue;
				}

				float ratioDenom = std::max(std::abs(vx), slipRatioDenom);
				float angleDenom = std::max(std::abs(vx), slipAngleDenom);

				float targetSlipRatio = clamp((surfaceSpeed - vx) / ratioDenom, -1.0f, 1.0f);
				float targetSlipAngle = clamp(std::atan2(vy, angleDenom), -0.55f, 0.55f);

				wheel.slipRatio += (targetSlipRatio - wheel.slipRatio) * relaxation;
				wheel.slipAngle += (targetSlipAngle - wheel.slipAngle) * relaxation;

				float maxGrip = tireMu * wheel.normalLoad;
				float lateralGripScale = clamp(wheelSpeed / lowSpeedLateralGripSpeed, 0.0f, 1.0f);
				float longitudinalGripScale = clamp(std::max(std::abs(surfaceSpeed), std::abs(vx)) / 75.0f, 0.0f, 1.0f);

				float fx = longitudinalCurve.evaluate(wheel.slipRatio) * maxGrip * longitudinalGripScale;
				float fy = -lateralCurve.evaluate(wheel.slipAngle) * maxGrip * lateralGripScale;
				float forceMag = std::sqrt(fx * fx + fy * fy);

				if (forceMag > maxGrip && forceMag > 0.0001f) {
					float scale = maxGrip / forceMag;
					fx *= scale;
					fy *= scale;
				}

				Vector2D tireForce = wf * fx + wr * fy;
				totalForce += tireForce;
				totalTorque += offset.x * tireForce.y - offset.y * tireForce.x;
			}
		}
		
		void createWheelEntities() {
			for (int i = 0; i < static_cast<int>(wheels.size()); i++) {
				WheelPhysics& wheel = wheels[i];
		
				Vector2D local = wheel.localPosition;
				Vector2D size = wheel.size;
		
				Entity& wheelEntity = manager->addEntity();
				wheelEntities.push_back(&wheelEntity);
		
				auto& wheelTransform = wheelEntity.addComponent<TransformComponent>(local.x,local.y,size.x,size.y);
				wheelTransform.setFather(transform);
				wheelEntity.addComponent<SpriteComponent>(wheelTexturePath);
				wheelEntity.addComponent<ColliderComponent>(wheelColliderTag,Polygon{{-size.x/2,-size.y/2},
																					 {size.x/2,-size.y/2},
																					 {size.x/2,size.y/2},
																					 {-size.x/2,size.y/2}});
																						 
				wheelEntity.addGroup(group);
			}
		}
		
		void setWheels(const std::vector<CarWheelConfig>& wheelConfigs) {
			wheels.clear();
			wheels.reserve(wheelConfigs.size());

			for (const CarWheelConfig& config : wheelConfigs) {
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
			}
		}
		
		void updateWheelEntities() {
			int count = std::min(
				static_cast<int>(wheels.size()),
				static_cast<int>(wheelEntities.size())
			);
	
			for (int i = 0; i < count; i++) {
				auto& wheelTransform = wheelEntities[i]->getComponent<TransformComponent>();
		
				wheelTransform.position = wheels[i].localPosition;
				wheelTransform.direction = wheels[i].steerAngle;
				wheelTransform.velocity = transform->velocity;
				wheelTransform.angularVelocity = transform->angularVelocity;
			}
		}
};
