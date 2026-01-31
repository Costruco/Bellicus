#ifndef SIMPLE_MOVEMENT_COMPONENT_HPP
#define SIMPLE_MOVEMENT_COMPONENT_HPP

#include "Components.hpp"
#include "Vector2D.hpp"
#include "MovementInput.hpp"
#include "Math.hpp"
#include "FrameManager.hpp"

class SimpleMovementComponent : public Component {
	public:
		TransformComponent * transform;
		float speed;
		float turningSpeed;
			   
		SimpleMovementComponent() {
			speed = 0;
		}
		SimpleMovementComponent(float speed, float turningSpeed) {
			this->speed = speed/1000;
			this->turningSpeed = turningSpeed/1000;
		}
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
			//direction
			if (transform->turnIntent == TurnDirection::LEFT)
				transform->direction += turningSpeed*FrameManager::getDeltaTime();
			else if (transform->turnIntent == TurnDirection::RIGHT)
				transform->direction -= turningSpeed*FrameManager::getDeltaTime();
			else
				;
			clockLimit(transform->direction,0.0f,360.0f);
			
			//movement
			if (transform->moveIntent == MovementDirection::FORWARD)
				transform->position += Vector2D::fromPolar(speed,transform->direction)*FrameManager::getDeltaTime();
			else if (transform->moveIntent == MovementDirection::BACKWARD)
				transform->position -= Vector2D::fromPolar(speed,transform->direction)*FrameManager::getDeltaTime();
			else
				;	
		}	
};

#endif
