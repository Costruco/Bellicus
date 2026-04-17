#pragma once

#include "Components.hpp"
#include "Vector2D.hpp"
#include "Input.hpp"
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
			float dt = FrameManager::getDeltaTime();
			
			//directionSign
			float directionSign = 0.0f;
			if (transform->moveIntent == MovementDirection::FORWARD)
				directionSign = 1.0f;
			else if (transform->moveIntent == MovementDirection::BACKWARD)
				directionSign = -1.0f;
			
			//angleOfMovement
			switch (transform->turnIntent) {
				case TurnDirection::STRAIGHT:
					break;
				case TurnDirection::LEFT:
					transform->direction -= turningSpeed*dt*((directionSign==0.0f)?1.0f:directionSign);
					break;
				case TurnDirection::RIGHT:
					transform->direction += turningSpeed*dt*((directionSign==0.0f)?1.0f:directionSign);
					break;
			}
			clockLimit(transform->direction,0.0f,360.0f);
			
			//movement
			if (directionSign != 0.0f) {
				Vector2D velocity = Vector2D::fromPolar(speed,transform->direction)*directionSign;
				transform->position += velocity*dt;
			}	
		}	
};
