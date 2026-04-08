#pragma once

#include "SDL2/SDL.h"

#include "Components.hpp"
#include "Map.hpp"
#include "Vector2D.hpp"
#include "MovementInput.hpp"
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
					if (transform->moveIntent != MovementDirection::BACKWARD) {
						wheelDirection += turningSpeed*dt;
						break;
					} else {
						wheelDirection -= turningSpeed*dt;
						break;
					}
				case TurnDirection::RIGHT:
					if (transform->moveIntent != MovementDirection::BACKWARD) {
						wheelDirection -= turningSpeed*dt;
						break;
					} else {
						wheelDirection += turningSpeed*dt;
						break;
					}
			}
			wheelDirection = clamp(wheelDirection,-30.0f,30.0f);
			
			
			
			//movement
			if (transform->moveIntent == MovementDirection::FORWARD) {
				transform->position += Vector2D::fromPolar(speed,transform->direction)*dt;
				transform->direction += speed/wheelBase*dt*tand(wheelDirection);
				clockLimit(transform->direction,0.0f,360.0f);
			} else if (transform->moveIntent == MovementDirection::BACKWARD) {
				transform->position -= Vector2D::fromPolar(speed,transform->direction)*dt;
				transform->direction += speed/wheelBase*dt*tand(wheelDirection);
				clockLimit(transform->direction,0.0f,360.0f);
			}
		}	
};
