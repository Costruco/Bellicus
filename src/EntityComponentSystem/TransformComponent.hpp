#pragma once

#include <algorithm>
#include <vector>

#include "Components.hpp"
#include "Utils/Math.hpp"
#include "Utils/Vector2D.hpp"

class TransformComponent : public Component {
	public:
		Vector2D position;
		Vector2D velocity;
		Vector2D acceleration;
		
		float direction = 0.0f;
		float angularVelocity = 0.0f;
		float angularAcceleration = 0.0f;
		
		float width = 0.0f;
		float height = 0.0f;
		float scale = 1.0f;
		Vector2D center_offset;

		TransformComponent() :
			position(), velocity(), acceleration(), center_offset() {
			direction = 0.0f;
			width = 0.0f;
			height = 0.0f;
			scale = 1.0f;
		}

		TransformComponent(float xpos, float ypos, float w, float h) :
			position(xpos, ypos), velocity(), acceleration(), center_offset() {
			direction = 0.0f;
			width = w;
			height = h;
			scale = 1.0f;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h) :
			position(xpos, ypos), velocity(), acceleration(), center_offset() {
			direction = angle;
			width = w;
			height = h;
			scale = 1.0f;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h, float sc) :
			position(xpos, ypos), velocity(), acceleration(), center_offset() {
			direction = angle;
			width = w;
			height = h;
			scale = sc;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h, float sc, float offx, float offy) :
			position(xpos, ypos), velocity(), acceleration(), center_offset(offx, offy) {
			direction = angle;
			width = w;
			height = h;
			scale = sc;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h, float offx, float offy) :
			position(xpos, ypos), velocity(), acceleration(), center_offset(offx, offy) {
			direction = angle;
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
			const Entity * fatherEntity = entity ? entity->getFather() : nullptr;
			if (fatherEntity && fatherEntity->hasComponent<TransformComponent>()) {
				const TransformComponent * fatherTransform = &fatherEntity->getComponent<TransformComponent>();
				return (position*fatherTransform->getScale()).rotate({0, 0},fatherTransform->getDirection())+fatherTransform->getPosition();
			}
			return position;
		}

		float getDirection() const {
			const Entity * fatherEntity = entity ? entity->getFather() : nullptr;
			if (fatherEntity && fatherEntity->hasComponent<TransformComponent>()) {
				const TransformComponent * fatherTransform = &fatherEntity->getComponent<TransformComponent>();
				return clockLimit(direction+fatherTransform->getDirection(),0.0f,360.0f);
			}
			return direction;
		}
		
		float getScale() const {
			const Entity * fatherEntity = entity ? entity->getFather() : nullptr;
			if (fatherEntity && fatherEntity->hasComponent<TransformComponent>()) {
				const TransformComponent * fatherTransform = &fatherEntity->getComponent<TransformComponent>();
				return scale*fatherTransform->getScale();
			}
			return scale;
		}

		Vector2D getVelocity() const {
			const Entity * fatherEntity = entity ? entity->getFather() : nullptr;
			if (fatherEntity && fatherEntity->hasComponent<TransformComponent>()) {
				const TransformComponent * fatherTransform = &fatherEntity->getComponent<TransformComponent>();
				float fatherDir = fatherTransform->getDirection();
				float fatherScale = fatherTransform->getScale();
				Vector2D r = (position * fatherScale).rotate({0, 0}, fatherDir);
				Vector2D tangentialVel = r.perpendicular() * (fatherTransform->getAngularVelocity() * toRad);
				Vector2D localWorldVel = (velocity * fatherScale).rotate({0, 0}, fatherDir);
				return fatherTransform->getVelocity() + tangentialVel + localWorldVel;
			}
			return velocity;
		}

		Vector2D getAcceleration() const {
			const Entity * fatherEntity = entity ? entity->getFather() : nullptr;
			if (fatherEntity && fatherEntity->hasComponent<TransformComponent>()) {
				const TransformComponent * fatherTransform = &fatherEntity->getComponent<TransformComponent>();
				float fatherDir = fatherTransform->getDirection();
				float fatherScale = fatherTransform->getScale();
				Vector2D r = (position * fatherScale).rotate({0, 0}, fatherDir);
				float omegaRad = fatherTransform->getAngularVelocity() * toRad;
				float alphaRad = fatherTransform->getAngularAcceleration() * toRad;
				Vector2D tangentialAcc = r.perpendicular() * alphaRad;
				Vector2D centripetalAcc = r * (-omegaRad * omegaRad);
				Vector2D localWorldVel = (velocity * fatherScale).rotate({0, 0}, fatherDir);
				Vector2D coriolisAcc = localWorldVel.perpendicular() * (2.0f * omegaRad);
				Vector2D localWorldAcc = (acceleration * fatherScale).rotate({0, 0}, fatherDir);
				return fatherTransform->getAcceleration() + tangentialAcc + centripetalAcc + coriolisAcc + localWorldAcc;
			}
			return acceleration;
		}

		float getAngularVelocity() const {
			const Entity * fatherEntity = entity ? entity->getFather() : nullptr;
			if (fatherEntity && fatherEntity->hasComponent<TransformComponent>()) {
				const TransformComponent * fatherTransform = &fatherEntity->getComponent<TransformComponent>();
				return angularVelocity + fatherTransform->getAngularVelocity();
			}
			return angularVelocity;
		}

		float getAngularAcceleration() const {
			const Entity * fatherEntity = entity ? entity->getFather() : nullptr;
			if (fatherEntity && fatherEntity->hasComponent<TransformComponent>()) {
				const TransformComponent * fatherTransform = &fatherEntity->getComponent<TransformComponent>();
				return angularAcceleration + fatherTransform->getAngularAcceleration();
			}
			return angularAcceleration;
		}

		float getSpeed() const {
			return getVelocity().getModule();
		}

		float getForwardSpeed() const {
			return getVelocity().dot(Vector2D::fromPolar(getDirection()));
		}
};