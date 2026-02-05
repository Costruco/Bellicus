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
			   
		SimpleMovementComponent(float speed, float turningSpeed) {
			this->speed = speed;
			this->turningSpeed = turningSpeed;
		}
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
			//direction
			if (transform->turnIntent == TurnDirection::LEFT)
				transform->direction += turningSpeed*FrameManager::getDeltaTime()/1000;
			else if (transform->turnIntent == TurnDirection::RIGHT)
				transform->direction -= turningSpeed*FrameManager::getDeltaTime()/1000;
			else
				;
			clockLimit(transform->direction,0.0f,360.0f);
			
			//movement
			if (transform->moveIntent == MovementDirection::FORWARD)
				transform->position += Vector2D::fromPolar(speed,transform->direction)*FrameManager::getDeltaTime()/1000;
			else if (transform->moveIntent == MovementDirection::BACKWARD)
				transform->position -= Vector2D::fromPolar(speed,transform->direction)*FrameManager::getDeltaTime()/1000;
			else
				;	
		}	
};

#endif
