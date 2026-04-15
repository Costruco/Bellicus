#pragma once

#include "SDL2/SDL.h"

#include "Components.hpp"
#include "Map.hpp"
#include "Vector2D.hpp"
#include "Input.hpp"
#include "Math.hpp"
#include "Physics.hpp"
#include "FrameManager.hpp"
#include <iterator>
#include <vector>
#include <iostream>

class CarMovementComponent : public Component {
	public:
		TransformComponent * transform;
		
		float speed;
		float wheelDirection;
		float turningSpeed;
		float wheelBase;
		
			   
		CarMovementComponent(float speed, float turningSpeed, float wheelBase) {
			this->speed = speed;
			this->turningSpeed = turningSpeed;
			this->wheelBase = wheelBase;
		}
		
		void init() override {
			wheelDirection = 0.0f;
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
			float dt = FrameManager::getDeltaTime();

			//direction
			switch (transform->turnIntent) {
				case TurnDirection::STRAIGHT:
					toZero(wheelDirection,turningSpeed*3*dt);
					break;
				case TurnDirection::LEFT:
					wheelDirection -= turningSpeed*dt;
					break;
				case TurnDirection::RIGHT:
					wheelDirection += turningSpeed*dt;
					break;
			}
			wheelDirection = clamp(wheelDirection,-30.0f,30.0f);
			
			//movement
			float directionSign = 0.0f;
			if (transform->moveIntent == MovementDirection::FORWARD)
				directionSign = 1.0f;
			else if (transform->moveIntent == MovementDirection::BACKWARD)
				directionSign = -1.0f;
			if (directionSign != 0.0f) {
				Vector2D velocity = Vector2D::fromPolar(speed,transform->direction)*directionSign;
				transform->position += velocity*dt;
				transform->direction += speed/wheelBase*tand(wheelDirection)*dt*directionSign;
				transform->direction = clockLimit(transform->direction,0.0f,360.0f);
			}
		}	
};
