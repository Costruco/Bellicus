#pragma once

#include <cstdint>

#include "EntityComponentSystem.hpp"
#include "TransformComponent.hpp"
#include "Input.hpp"
#include "Utils/Math.hpp"
#include "Utils/Vector2D.hpp"

class MovementComponent : public Component {
	protected:
		enum class LinearCommand : std::uint8_t {
			INTENT = 0,
			MOVE_TO,
			WALK_TO,
			HOLD_POSITION
		};

		enum class AngularCommand : std::uint8_t {
			INTENT = 0,
			TURN_TO,
			HOLD_DIRECTION
		};

		LinearCommand linearCommand = LinearCommand::INTENT;
		AngularCommand angularCommand = AngularCommand::INTENT;
		Vector2D targetPosition;
		Vector2D targetLookPosition;
		float targetDirection = 0.0f;
		bool turnTargetIsPoint = false;

		void resolveTargets() {
			if (!transform)
				return;
			Vector2D pos = transform->getPosition();
			if (linearCommand == LinearCommand::MOVE_TO) {
				if (targetPosition != pos) {
					targetDirection = clockLimit(180.0f + angleBetweenPoints(pos, targetPosition), 0.0f, 360.0f);
					angularCommand = AngularCommand::TURN_TO;
				} else {
					angularCommand = AngularCommand::HOLD_DIRECTION;
				}
			} else if (angularCommand == AngularCommand::TURN_TO && turnTargetIsPoint) {
				if (targetLookPosition != pos) {
					targetDirection = clockLimit(180.0f + angleBetweenPoints(pos, targetLookPosition), 0.0f, 360.0f);
				} else {
					angularCommand = AngularCommand::HOLD_DIRECTION;
				}
			}
		}

		void resetCommands() {
			linearCommand = LinearCommand::INTENT;
			angularCommand = AngularCommand::INTENT;
			turnTargetIsPoint = false;
		}

	public:
		using ComponentInterface = MovementComponent;

		TransformComponent * transform = nullptr;
		MovementDirection moveIntent = MovementDirection::STILL;
		TurnDirection turnIntent = TurnDirection::STRAIGHT;

		virtual ~MovementComponent() = default;

		void init() override {
			if (!entity->hasComponent<TransformComponent>())
				entity->addComponent<TransformComponent>();
			transform = &entity->getComponent<TransformComponent>();
		}

		virtual void moveTo(const Vector2D& target) {
			targetPosition = target;
			linearCommand = LinearCommand::MOVE_TO;
			turnTargetIsPoint = false;
			if (transform) {
				Vector2D pos = transform->getPosition();
				if (target != pos) {
					targetDirection = clockLimit(180.0f + angleBetweenPoints(pos, target), 0.0f, 360.0f);
					angularCommand = AngularCommand::TURN_TO;
				} else {
					angularCommand = AngularCommand::HOLD_DIRECTION;
				}
			} else {
				angularCommand = AngularCommand::TURN_TO;
			}
		}

		void moveTo(float x, float y) {
			moveTo(Vector2D(x, y));
		}

		virtual void turnTo(float direction) {
			targetDirection = clockLimit(direction, 0.0f, 360.0f);
			turnTargetIsPoint = false;
			angularCommand = AngularCommand::TURN_TO;
			if (linearCommand == LinearCommand::INTENT || linearCommand == LinearCommand::MOVE_TO)
				linearCommand = LinearCommand::HOLD_POSITION;
		}

		virtual void turnTo(const Vector2D& target) {
			targetLookPosition = target;
			turnTargetIsPoint = true;
			angularCommand = AngularCommand::TURN_TO;
			if (linearCommand == LinearCommand::INTENT || linearCommand == LinearCommand::MOVE_TO)
				linearCommand = LinearCommand::HOLD_POSITION;
			if (transform) {
				Vector2D pos = transform->getPosition();
				if (target != pos)
					targetDirection = clockLimit(180.0f + angleBetweenPoints(pos, target), 0.0f, 360.0f);
			}
		}

		void turnTo(float x, float y) {
			turnTo(Vector2D(x, y));
		}

		virtual void walkTo(const Vector2D& target) {
			targetPosition = target;
			linearCommand = LinearCommand::WALK_TO;
			if (angularCommand == AngularCommand::INTENT)
				angularCommand = AngularCommand::HOLD_DIRECTION;
		}

		void walkTo(float x, float y) {
			walkTo(Vector2D(x, y));
		}
};
