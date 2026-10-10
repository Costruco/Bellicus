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
#include <SDL2/SDL2_gfxPrimitives.h>
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
			group(group),
			manager(manager),
			wheelTexturePath(path),
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
			groundDrag(groundDrag),
			downforce(downforce),
			yawDamping(yawDamping),
			tireRelaxation(tireRelaxation),
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
			VehicleControl input = getInput();
			int steps = std::max(1,static_cast<int>(std::ceil(frameDt/maxSubstep)));
			float dt = frameDt/steps;

			for (int i = 0; i < steps; i++)
				step(dt, input);

			updateWheelEntities();
		}

		float getSpeed() const {
			return velocity.getModule();
		}

	private:
		void step(float dt, const VehicleControl& input) {
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

			float speed = getSpeed();
			if (speed > 0.0f)
				velocity = velocity.getDirection()*std::max(0.0f,speed-groundDrag*GRAVITY*dt);

			yawRate += angularAcceleration * dt;
			yawRate = moveToward(yawRate, 0.0f, yawDamping*dt);
			if (getSpeed() < 0.1f)
				yawRate = 0.0f;
			transform->position += velocity*dt;
			transform->direction = clockLimit(transform->direction+yawRate*toDeg*dt,0.0f,360.0f);
			transform->velocity = velocity;
			transform->angularVelocity = yawRate * toDeg;
			
			previousAcceleration = acceleration;
		}

		VehicleControl getInput() {
			VehicleControl input;
		
			input.steer = static_cast<int>(transform->turnIntent);
			if (transform->moveIntent == MovementDirection::STILL)
				return input;

			float forwardSpeed = velocity.dot(forward());
			float speed = getSpeed();

			if (transform->moveIntent == MovementDirection::FORWARD) {
				if (gearbox.gear == 0 && forwardSpeed < -10.0f)
					input.brake = 1.0f;
				else
					input.throttle = 1.0f;
			}
			else if (transform->moveIntent == MovementDirection::BACKWARD) {
				if (gearbox.gear == 0 || (speed < 15.0f && forwardSpeed <= 5.0f))
					input.throttle = 1.0f;
				else
					input.brake = 1.0f;
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
		
			return torqueCurve.getTorque(rpm)*std::abs(ratio);
		}

		void updateTransmission(float dt, const VehicleControl& input) {
			if (shiftCooldown > 0.0f)
				shiftCooldown -= dt;
		
			float forwardSpeed = velocity.dot(forward());
			float speed = getSpeed();
		
			if (transform->moveIntent == MovementDirection::STILL && speed < 5.0f) {
				gearbox.setGear(1);
				return;
			}
		
			if (transform->moveIntent == MovementDirection::BACKWARD && (gearbox.gear == 0 || (speed < 15.0f && forwardSpeed <= 5.0f))) {
				gearbox.setGear(0);
				return;
			}

			if (transform->moveIntent == MovementDirection::FORWARD && gearbox.gear < 2 && forwardSpeed >= -10.0f)
				gearbox.setGear(2);

			if (gearbox.gear < 2 || shiftCooldown > 0.0f)
				return;
		
			int currentGear = gearbox.gear;
			int nextGear = currentGear+1;
			int previousGear = currentGear-1;
			float wheelRPM = clamp(std::abs(radToRpm((forwardSpeed/wheelRadius)*gearbox.totalRatio())),idleRPM,maxRPM);
		
			if (input.throttle > 0.0f && nextGear <= gearbox.maxGear() && engineRPM >= maxRPM*0.86f) {
				float nextRPM = rpmAfterShift(nextGear);
				gearbox.shiftUp();
				engineRPM = nextRPM;
				shiftCooldown = shiftDelay;
				return;
			}
		
			if (previousGear >= 2 && std::min(engineRPM,wheelRPM) <= idleRPM+(maxRPM-idleRPM)*0.35f) {
				float previousRPM = clamp(std::min(engineRPM,wheelRPM)*std::abs(gearRatioAt(previousGear)/gearRatioAt(currentGear)),idleRPM,maxRPM);
				if (previousRPM <= maxRPM*0.80f) {
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

		void updateNormalLoads() {
			if (wheels.empty())
				return;

			float speedM = getSpeed()/PIXELS_PER_METER;
			float totalWeight = weight()+downforce*speedM*speedM*PIXELS_PER_METER;
			float longAcc = clamp(previousAcceleration.dot(forward()),-1.5f*GRAVITY,1.5f*GRAVITY);
			float latAcc = clamp(previousAcceleration.dot(right()),-1.5f*GRAVITY,1.5f*GRAVITY);
			float longTransfer = mass*longAcc*cgHeight/wheelBase;
			float latTransfer = mass*latAcc*cgHeight/trackWidth;
			float baseLoad = totalWeight/wheels.size();

			for (WheelPhysics& wheel : wheels) {
				float load = baseLoad;
				load += (wheel.localPosition.x >= 0.0f)?(-0.5f*longTransfer):(0.5f*longTransfer);
				load += (wheel.localPosition.y >= 0.0f)?(-0.5f*latTransfer):(0.5f*latTransfer);
				wheel.normalLoad = clamp(load,totalWeight*0.05f,totalWeight*0.60f);
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
			return sum/count;
		}

		void updateEngineAndDrivenWheels(float dt, const VehicleControl& input) {
			float torque = torqueCurve.getTorque(engineRPM);
			float torqueScale = torque/std::max(1.0f,torqueCurve.maxTorque);
			float ratio = gearbox.totalRatio();
			float rpmSpan = maxRPM-idleRPM;
		
			if (gearbox.inNeutral()) {
				float friction = (engineRPM-idleRPM)*engineFriction;
				float dRPM = input.throttle*torqueScale*rpmSpan*engineThrottleGain-friction;
				engineRPM = clamp(engineRPM+dRPM*dt,idleRPM,maxRPM);
				return;
			}
		
			float drivenOmega = averageDrivenWheelOmega();
			float expectedEngineRPM = clamp(std::abs(radToRpm(drivenOmega*ratio)),idleRPM,maxRPM);
		
			float throttlePull = input.throttle*torqueScale*rpmSpan*(engineThrottleGain*0.45f);
			float clutchPull = (expectedEngineRPM-engineRPM)*(engineSyncGain*3.5f);
			float friction = (engineRPM-expectedEngineRPM)*engineFriction+(1.0f-input.throttle)*(engineRPM-idleRPM)*(engineFriction*0.35f);
		
			engineRPM = clamp(engineRPM+(throttlePull+clutchPull-friction)*dt,idleRPM,maxRPM);
		
			float expectedWheelOmega = rpmToRad(engineRPM)/ratio;
			float clutchScale = 0.25f+0.75f*input.throttle;
		
			for (WheelPhysics& wheel : wheels) {
				if (!wheel.driven)
					continue;
		
				float dOmega = wheelSyncGain*torqueScale*clutchScale*(expectedWheelOmega-wheel.omega);
				wheel.omega += dOmega * dt;
			}
		}

		void updateFreeWheelsAndBrakes(float dt, const VehicleControl& input) {
			float gearDir = (gearbox.gear == 0)?-1.0f:1.0f;

			for (WheelPhysics& wheel : wheels) {
				Vector2D offset = localToWorld(wheel.localPosition);
				Vector2D wheelVelocity = pointVelocity(offset);
				Vector2D wf = wheelForward(wheel);
				float longitudinalSpeed = wheelVelocity.dot(wf);
				float freeRollingOmega = longitudinalSpeed / wheelRadius;

				if (!wheel.driven || gearbox.inNeutral()) {
					if (input.brake <= 0.0f)
						wheel.omega = freeRollingOmega;
					else
						wheel.omega += (freeRollingOmega - wheel.omega) * freeWheelFollow * dt;
				}
				else {
					float targetFro = (gearDir > 0.0f)?std::max(0.0f,freeRollingOmega):std::min(0.0f,freeRollingOmega);
					float roadFollow = freeWheelFollow*(1.0f-0.45f*input.throttle);
					wheel.omega += (targetFro - wheel.omega) * roadFollow * dt;
				}

				if (input.brake > 0.0f) {
					float brakeScale = (wheel.steerable && std::abs(wheelDirection) > 1.0f)?0.55f:1.0f;
					wheel.omega = moveToward(wheel.omega, 0.0f, brakeAngularDecel * input.brake * brakeScale * dt);
				}
				wheel.omega -= wheel.omega * wheelAngularDrag * dt;
			}
		}

		void addTireForces(float dt, Vector2D& totalForce, float& totalTorque) {
			float relaxation = clamp(tireRelaxation * dt, 0.0f, 1.0f);
			Vector2D f = forward();
			Vector2D r = right();
			float speed = getSpeed();
			float forwardSpeed = velocity.dot(f);
			float gearDir = (gearbox.gear == 0)?-1.0f:1.0f;
			float alignedSpeed = forwardSpeed*gearDir;
			float slideRatio = clamp((speed*0.65f-std::max(0.0f,alignedSpeed))/std::max(speed*0.65f,slipAngleDenom),0.0f,1.0f);
			float steerTurnRatio = std::abs(wheelDirection)/std::max(1.0f,maxSteerAngle);
			float avgGrip = tireMu*(weight()/std::max<size_t>(1,wheels.size()));

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

				float fx = longitudinalCurve.evaluate(wheel.slipRatio) * maxGrip * std::max(0.20f,1.0f-1.35f*std::abs(wheel.slipAngle));
				float fy = -lateralCurve.evaluate(wheel.slipAngle) * maxGrip * (1.0f-0.35f*std::abs(wheel.slipRatio)*steerTurnRatio);
				if (!wheel.steerable)
					fy *= (1.0f-0.30f*steerTurnRatio*clamp(speed/200.0f,0.0f,1.0f));
				float forceMag = std::sqrt(fx * fx + fy * fy);

				if (forceMag > maxGrip && forceMag > 0.0001f) {
					float scale = maxGrip / forceMag;
					fx *= scale;
					fy *= scale;
				}

				Vector2D rollingForce = wf * fx + (wheel.steerable ? r : wr) * fy;
				Vector2D rotVel = offset.perpendicular() * yawRate;
				Vector2D kineticDrag = velocity.getDirection()*(-0.35f*avgGrip) - rotVel.getDirection()*(0.32f*avgGrip*clamp(rotVel.getModule()/160.0f,0.0f,1.0f));
				Vector2D steerForce = wheel.steerable ? (r * sind(wheel.steerAngle * gearDir) * (0.88f * avgGrip * clamp(speed / 120.0f, 0.0f, 1.0f))) : Vector2D();
				Vector2D driveSlide = f * (fx * 0.55f);
				Vector2D kineticForce = kineticDrag + steerForce + driveSlide;

				Vector2D tireForce = rollingForce * (1.0f - slideRatio) + kineticForce * slideRatio;
				totalForce += tireForce;
				totalTorque += offset.x * tireForce.y - offset.y * tireForce.x;
			}
		}
		
		void createWheelEntities() {
			Vector2D defaultWheelSize;
			if (wheels.size())
				defaultWheelSize = wheels[0].size;
			for (int i = 0; i < wheels.size(); i++) {
				WheelPhysics& wheel = wheels[i];
				
				float x = (i<wheels.size()/2)?wheelBase*0.5f:-wheelBase*0.5f;
				float y = (i%2==0)?-trackWidth*0.5f:trackWidth*0.5f;
				wheel.localPosition = (wheel.localPosition==Vector2D())?Vector2D(x,y):wheel.localPosition;
				Vector2D local = wheel.localPosition;
				wheel.size = (wheel.size==Vector2D())?defaultWheelSize:wheel.size;
				Vector2D size = wheel.size;
		
				Entity& wheelEntity = manager->addEntity();
				wheelEntities.push_back(&wheelEntity);
				auto& wheelTransform = wheelEntity.addComponent<TransformComponent>(local.x,local.y,size.x,size.y);
				wheelTransform.entity->setFather(entity);
				wheelEntity.addComponent<SpriteComponent>(wheelTexturePath);
				wheelEntity.addComponent<ColliderComponent>(wheelColliderTag);																 
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