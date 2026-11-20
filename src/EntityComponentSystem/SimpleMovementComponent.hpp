#pragma once

#include <algorithm>
#include <cmath>

#include "Components.hpp"
#include "FrameManager.hpp"
#include "Input.hpp"
#include "MovementComponent.hpp"
#include "Utils/Math.hpp"
#include "Utils/Vector2D.hpp"

class SimpleMovementComponent : public MovementComponent {
	public:
		float maxSpeed = 0.0f;
		float turningSpeed = 0.0f;

		SimpleMovementComponent() = default;
		SimpleMovementComponent(float maxSpeed, float turningSpeed) {
			this->maxSpeed = maxSpeed;
			this->turningSpeed = turningSpeed;
		}
		
		void update() override {
			float dt = FrameManager::getDeltaTime();
			if (dt <= 0.0f || !transform) {
				resetCommands();
				return;
			}

			resolveTargets();

			Vector2D previousVelocity = transform->velocity;
			float previousAngularVelocity = transform->angularVelocity;

			float directionSign = 0.0f;
			if (linearCommand == LinearCommand::INTENT) {
				if (moveIntent == MovementDirection::FORWARD)
					directionSign = 1.0f;
				else if (moveIntent == MovementDirection::BACKWARD)
					directionSign = -1.0f;
			}

			// Angular movement
			float angleStep = 0.0f;
			if (angularCommand == AngularCommand::TURN_TO) {
				float currentWorldDir = transform->getDirection();
				float angleDiff = shortestAngleDiff(currentWorldDir, targetDirection);
				float maxTurn = turningSpeed * dt;
				angleStep = clamp(angleDiff, -maxTurn, maxTurn);
			} else if (angularCommand == AngularCommand::INTENT) {
				float turnFactor = (directionSign == 0.0f) ? 1.0f : directionSign;
				switch (turnIntent) {
					case TurnDirection::STRAIGHT:
					case TurnDirection::NONE:
						break;
					case TurnDirection::LEFT:
						angleStep = -turningSpeed * dt * turnFactor;
						break;
					case TurnDirection::RIGHT:
						angleStep = turningSpeed * dt * turnFactor;
						break;
				}
			}

			transform->direction = clockLimit(transform->direction + angleStep, 0.0f, 360.0f);
			float newAngularVelocity = angleStep / dt;
			transform->angularAcceleration = (newAngularVelocity - previousAngularVelocity) / dt;
			transform->angularVelocity = newAngularVelocity;

			// Linear movement
			Vector2D newVelocity;
			if (linearCommand == LinearCommand::WALK_TO) {
				Vector2D toTarget = targetPosition - transform->getPosition();
				float dist = toTarget.getModule();
				float maxStep = maxSpeed * dt;
				if (dist > 1e-5f && maxStep > 0.0f) {
					Vector2D worldStep = (dist <= maxStep) ? toTarget : (toTarget / dist) * maxStep;
					newVelocity = worldStep / dt;
				}
			} else if (linearCommand == LinearCommand::MOVE_TO) {
				Vector2D toTarget = targetPosition - transform->getPosition();
				float dist = toTarget.getModule();
				float maxStep = maxSpeed * dt;
				if (dist > 1e-5f && maxStep > 0.0f) {
					Vector2D forward = Vector2D::fromPolar(transform->getDirection());
					float forwardProj = toTarget.dot(forward);
					if (turningSpeed <= 0.0f) {
						Vector2D worldStep = (dist <= maxStep) ? toTarget : (toTarget / dist) * maxStep;
						newVelocity = worldStep / dt;
					} else if (forwardProj > 0.0f) {
						float stepMag = std::min(maxStep, forwardProj);
						newVelocity = forward * (stepMag / dt);
					}
				}
			} else if (linearCommand == LinearCommand::INTENT) {
				if (directionSign != 0.0f) {
					newVelocity = Vector2D::fromPolar(transform->getDirection(), maxSpeed) * directionSign;
				}
			}

			Vector2D worldStep = newVelocity * dt;
			const Entity * fatherEntity = entity->getFather();
			if (fatherEntity && fatherEntity->hasComponent<TransformComponent>()) {
				const auto& fatherTransform = fatherEntity->getComponent<TransformComponent>();
				float fatherScale = fatherTransform.getScale();
				if (fatherScale > 1e-5f) {
					Vector2D localStep = (worldStep / fatherScale).rotate({0, 0}, -fatherTransform.getDirection());
					transform->position += localStep;
					transform->velocity = localStep / dt;
					transform->acceleration = (transform->velocity - previousVelocity) / dt;
				}
			} else {
				transform->position += worldStep;
				transform->velocity = newVelocity;
				transform->acceleration = (newVelocity - previousVelocity) / dt;
			}

			resetCommands();
		}	
};
