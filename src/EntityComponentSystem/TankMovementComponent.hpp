#pragma once

#include <SDL2/SDL.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <vector>

#include "Components.hpp"
#include "FrameManager.hpp"
#include "Input.hpp"
#include "MovementComponent.hpp"
#include "Utils/Math.hpp"
#include "Utils/Physics.hpp"
#include "Utils/Vector2D.hpp"

//{0.25f,0.17f}

class TankMovementComponent : public MovementComponent {
	private:
		float initialSpeed = 0.0f;

	public:
		float turningSpeed = 0.0f;
		int enginePower = 0; 
		int mass = 1;
		int brakePower = 0;
		float rollingDrag = 0.0f;
		
		//gearSystem
		Uint32 gearChangeStart = 0;
		bool increasing = false;
		bool decreasing = false;
		
		int nGears = 0;
		std::vector<Vector2D> gearsMaxSpeed;
		int neutralGear = 0;
		int currentGear = 0;
			   
		TankMovementComponent() = default;

		TankMovementComponent(float turningSpeed, int enginePower, int mass, int brakePower, float rollingDrag, int nGears, 
		std::vector<Vector2D> maxSpeedArray) : gearsMaxSpeed(std::move(maxSpeedArray)) {
			this->turningSpeed = turningSpeed;
			this->enginePower = enginePower;
			this->mass = mass;
			this->brakePower = brakePower;
			this->rollingDrag = rollingDrag;
			
			//gearSystem
			this->nGears = nGears;
			for (int i = 0; i < nGears; i++) {
				if (gearsMaxSpeed[i].x == 0 && gearsMaxSpeed[i].y == 0) {
					currentGear = neutralGear = i;
					break;
				}
			}
			increasing = false;
			decreasing = false;
		}

		TankMovementComponent(float initialSpeed, float turningSpeed, int enginePower, int mass, int brakePower, float rollingDrag, int nGears, 
		std::vector<Vector2D> maxSpeedArray) :
			TankMovementComponent(turningSpeed, enginePower, mass, brakePower, rollingDrag, nGears, std::move(maxSpeedArray)) {
			this->initialSpeed = initialSpeed;
		}
		
		void init() override {
			MovementComponent::init();
			if (initialSpeed != 0.0f && transform) {
				transform->velocity = Vector2D::fromPolar(transform->direction, initialSpeed);
			}
		}
		
		void update() override {
			float groundDrag = 0.25f;
			float dt = FrameManager::getDeltaTime();
			if (dt <= 0.0f || !transform) {
				resetCommands();
				return;
			}

			resolveTargets();

			Vector2D previousVelocity = transform->velocity;
			float previousAngularVelocity = transform->angularVelocity;

			Vector2D forward = Vector2D::fromPolar(transform->direction);
			float speed = transform->velocity.dot(forward);
			if (std::abs(speed) < 1e-4f)
				speed = 0.0f;
			else if (!gearsMaxSpeed.empty()) {
				float gMin = gearsMaxSpeed[currentGear].x;
				float gMax = gearsMaxSpeed[currentGear].y;
				if (std::abs(speed - gMax) < 1e-3f)
					speed = gMax;
				else if (std::abs(speed - gMin) < 1e-3f)
					speed = gMin;
			}

			float brakeDecel = desacc(brakePower, mass) * PIXELS_PER_METER;
			float stoppingDist = (brakeDecel > 1e-5f) ? ((speed * speed) / (2.0f * brakeDecel)) : 0.0f;

			// Resolve angular step
			float angleStep = 0.0f;
			if (angularCommand == AngularCommand::TURN_TO) {
				float angleDiff = shortestAngleDiff(transform->getDirection(), targetDirection);
				float maxTurn = turningSpeed * dt;
				angleStep = clamp(angleDiff, -maxTurn, maxTurn);
			} else if (angularCommand == AngularCommand::INTENT) {
				switch (turnIntent) {
					case TurnDirection::STRAIGHT:
					case TurnDirection::NONE:
						break;
					case TurnDirection::LEFT:
						angleStep = -turningSpeed * dt;
						break;
					case TurnDirection::RIGHT:
						angleStep = turningSpeed * dt;
						break;
				}
			}

			// Resolve longitudinal movement intent
			auto computeLongitudinalIntent = [&](float longitudinalError) -> MovementDirection {
				if (std::abs(longitudinalError) <= 1.0f && std::abs(speed) <= brakeDecel * dt) {
					speed = 0.0f;
					return MovementDirection::STILL;
				}
				if (longitudinalError * speed > 0.0f && std::abs(longitudinalError) <= stoppingDist + std::abs(speed) * dt) {
					return MovementDirection::BRAKE;
				}
				if (longitudinalError > 0.5f)
					return MovementDirection::FORWARD;
				if (longitudinalError < -0.5f)
					return MovementDirection::BACKWARD;
				return MovementDirection::STILL;
			};

			MovementDirection effectiveMoveIntent = moveIntent;
			if (linearCommand == LinearCommand::HOLD_POSITION) {
				effectiveMoveIntent = (std::abs(speed) > 1e-4f) ? MovementDirection::BRAKE : MovementDirection::STILL;
			} else if (linearCommand == LinearCommand::WALK_TO) {
				float longitudinalError = (targetPosition - transform->getPosition()).dot(forward);
				effectiveMoveIntent = computeLongitudinalIntent(longitudinalError);
			} else if (linearCommand == LinearCommand::MOVE_TO) {
				Vector2D toTarget = targetPosition - transform->getPosition();
				float dist = toTarget.getModule();
				if (dist <= 2.0f) {
					angleStep = 0.0f;
					if (std::abs(speed) <= brakeDecel * dt) {
						speed = 0.0f;
						effectiveMoveIntent = MovementDirection::STILL;
					} else {
						effectiveMoveIntent = MovementDirection::BRAKE;
					}
				} else {
					float angleDiff = std::abs(shortestAngleDiff(transform->getDirection(), targetDirection));
					if (angleDiff > 45.0f) {
						effectiveMoveIntent = (std::abs(speed) > 1e-4f) ? MovementDirection::BRAKE : MovementDirection::STILL;
					} else {
						float longitudinalError = toTarget.dot(forward);
						effectiveMoveIntent = computeLongitudinalIntent(longitudinalError);
					}
				}
			}
			
			// Atualiza a currentGear do tanque com base na currentGear atual, speed e controle
			float gearMin = gearsMaxSpeed[currentGear].x;
			float gearMax = gearsMaxSpeed[currentGear].y;
			if (currentGear == neutralGear) {
				if (aproxEqual(speed,gearMax) && increasing == false && effectiveMoveIntent == MovementDirection::FORWARD) {
					gearChangeStart = SDL_GetTicks();
					increasing = true;
				} else if (aproxEqual(speed,gearMin) && decreasing == false && effectiveMoveIntent == MovementDirection::BACKWARD) {
					gearChangeStart = SDL_GetTicks();
					decreasing = true;
				}
			} else {
				if (currentGear != nGears-1 && aproxEqual(speed,gearMax) && increasing == false && effectiveMoveIntent == MovementDirection::FORWARD) {
					gearChangeStart = SDL_GetTicks();
					increasing = true;
				} else if (currentGear > neutralGear && aproxEqual(speed,gearMin) && decreasing == false && (effectiveMoveIntent == MovementDirection::BACKWARD || effectiveMoveIntent == MovementDirection::BRAKE)) {
					currentGear = std::max(currentGear-1,neutralGear);
				} else if (currentGear > neutralGear && aproxEqual(speed,gearMin) && decreasing == false && effectiveMoveIntent == MovementDirection::STILL) {
					gearChangeStart = SDL_GetTicks();
					decreasing = true;
				}
			}
			
			if (increasing == true && SDL_GetTicks()-gearChangeStart >= 250) {
				currentGear = std::min(currentGear+1,nGears-1);
				increasing = false;
			} else if (decreasing == true && SDL_GetTicks()-gearChangeStart >= 250) {
				currentGear = std::max(currentGear-1,0);
				decreasing = false;
			}
			
			// Movement
			int troca = (increasing == true || decreasing == true);
			bool isTurning = (std::abs(angleStep) > 1e-4f);
			double TURNING = 0.035 * isTurning;
			gearMin = gearsMaxSpeed[currentGear].x;
			gearMax = gearsMaxSpeed[currentGear].y;
			if (speed < 0.0f) {
				if (effectiveMoveIntent == MovementDirection::STILL || effectiveMoveIntent == MovementDirection::BRAKE)
					speed = std::min(0.0f, std::min(gearMax, speed + brakeDecel * dt));
				else if (effectiveMoveIntent == MovementDirection::FORWARD)
					speed = std::min(0.0f, std::min(gearMax, speed + brakeDecel * dt));
				else if (effectiveMoveIntent == MovementDirection::BACKWARD && !troca)
					speed = std::max(gearMin, speed + acc(speed, mass, enginePower, groundDrag, rollingDrag + TURNING) * dt);
			} else if (speed == 0.0f) {
				if (effectiveMoveIntent == MovementDirection::FORWARD && !troca)
					speed = std::max(gearMin, speed + acc(speed, mass, enginePower, groundDrag, rollingDrag + TURNING) * dt);
				else if (effectiveMoveIntent == MovementDirection::BACKWARD && !troca)
					speed = std::min(gearMax, speed - acc(speed, mass, enginePower, groundDrag, rollingDrag + TURNING) * dt);
			} else if (speed > 0.0f) {
				if (effectiveMoveIntent == MovementDirection::STILL || effectiveMoveIntent == MovementDirection::BRAKE)
					speed = std::max(0.0f, std::max(gearMin, speed - brakeDecel * dt));
				else if (effectiveMoveIntent == MovementDirection::FORWARD && !troca)
					speed = std::min(gearMax, speed + acc(speed, mass, enginePower, groundDrag, rollingDrag + TURNING) * dt);
				else if (effectiveMoveIntent == MovementDirection::BACKWARD)
					speed = std::max(0.0f, std::max(gearMin, speed - brakeDecel * dt));
			}

			// Direction
			transform->direction = clockLimit(transform->direction + angleStep, 0.0f, 360.0f);
			float newAngularVelocity = angleStep / dt;
			transform->angularAcceleration = (newAngularVelocity - previousAngularVelocity) / dt;
			transform->angularVelocity = newAngularVelocity;

			// Update velocity, acceleration, and position in TransformComponent
			Vector2D newVelocity = Vector2D::fromPolar(transform->direction, speed);
			transform->acceleration = (newVelocity - previousVelocity) / dt;
			transform->velocity = newVelocity;
			transform->position += transform->velocity * dt;

			resetCommands();
		}	
};
