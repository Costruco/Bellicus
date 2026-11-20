#pragma once

#include <algorithm>
#include <vector>

#include "Components.hpp"
#include "Input.hpp"
#include "Utils/Math.hpp"
#include "Utils/Vector2D.hpp"

class TransformComponent : public Component {
	public:
		Vector2D position;
		Vector2D velocity;
		
		float direction;
		float angularVelocity = 0.0f;
		
		MovementDirection moveIntent;
		TurnDirection turnIntent;
		
		float width;
		float height;
		float scale;
		Vector2D center_offset;

		TransformComponent() :
			position(), velocity(), center_offset() {
			direction = 0.0f;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = 0.0f;
			height = 0.0f;
			scale = 1.0f;
		}

		TransformComponent(float xpos, float ypos, float w, float h) :
			position(xpos, ypos), velocity(), center_offset() {
			direction = 0.0f;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = 1.0f;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h) :
			position(xpos, ypos), velocity(), center_offset() {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = 1.0f;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h, float sc) :
			position(xpos, ypos), velocity(), center_offset() {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = sc;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h, float sc, float offx, float offy) :
			position(xpos, ypos), velocity(), center_offset(offx, offy) {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = sc;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h, float offx, float offy) :
			position(xpos, ypos), velocity(), center_offset(offx, offy) {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = 1.0f;
		}

		TransformComponent(Vector2D pos, float w, float h) :
			TransformComponent(pos.x, pos.y, w, h) {}

		TransformComponent(Vector2D pos, float angle, float w, float h) :
			TransformComponent(pos.x, pos.y, angle, w, h) {}

		TransformComponent(Vector2D pos, float angle, float w, float h, float sc) :
			TransformComponent(pos.x, pos.y, angle, w, h, sc) {}

		TransformComponent(Vector2D pos, float angle, float w, float h, float sc, Vector2D offset) :
			TransformComponent(pos.x, pos.y, angle, w, h, sc, offset.x, offset.y) {}

		TransformComponent(Vector2D pos, float angle, float w, float h, Vector2D offset) :
			TransformComponent(pos.x, pos.y, angle, w, h, offset.x, offset.y) {}
		
		Vector2D getPosition() const {
			const Entity * fatherEntity = entity->getFather();
			if (fatherEntity) {
				TransformComponent * fatherTransform = &fatherEntity->getComponent<TransformComponent>();
				return (position*fatherTransform->getScale()).rotate({0, 0},fatherTransform->getDirection())+fatherTransform->getPosition();
			}
			return position;
		}

		float getDirection() const {
			const Entity * fatherEntity = entity->getFather();
			if (fatherEntity) {
				TransformComponent * fatherTransform = &fatherEntity->getComponent<TransformComponent>();
				return clockLimit(direction+fatherTransform->getDirection(),0.0f,360.0f);
			}
			return direction;
		}
		
		float getScale() const {
			const Entity * fatherEntity = entity->getFather();
			if (fatherEntity) {
				TransformComponent * fatherTransform = &fatherEntity->getComponent<TransformComponent>();
				return scale*fatherTransform->getScale();
			}
			return scale;
		}
};