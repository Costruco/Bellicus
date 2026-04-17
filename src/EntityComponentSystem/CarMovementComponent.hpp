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
		    
		    // directionSign
		    float directionSign = 0.0f;
		    if (transform->moveIntent == MovementDirection::FORWARD)
		        directionSign = 1.0f;
		    else if (transform->moveIntent == MovementDirection::BACKWARD)
		        directionSign = -1.0f;
		    
		    // steeringAngle
		    switch (transform->turnIntent) {
		        case TurnDirection::STRAIGHT:
		            toZero(wheelDirection, turningSpeed * 3 * dt);
		            break;
		        case TurnDirection::LEFT:
		            wheelDirection -= turningSpeed * dt;
		            break;
		        case TurnDirection::RIGHT:
		            wheelDirection += turningSpeed * dt;
		            break;
		    }
		    wheelDirection = clamp(wheelDirection, -35.0f, 35.0f);
			
			//angleOfMovement and movement
		    if (directionSign != 0.0f) {
		        Vector2D rearAxle  = transform->position + Vector2D::fromPolar(-wheelBase / 2.0f, transform->direction);
		        Vector2D frontAxle = transform->position + Vector2D::fromPolar( wheelBase / 2.0f, transform->direction);
		
		        rearAxle  += Vector2D::fromPolar(speed * dt * directionSign, transform->direction);
		        frontAxle += Vector2D::fromPolar(speed * dt * directionSign, transform->direction + wheelDirection);
		
		        Vector2D headingVec = frontAxle - rearAxle;
		        transform->direction = clockLimit(headingVec.angle(), 0.0f, 360.0f);
		
		        transform->position = rearAxle + Vector2D::fromPolar(wheelBase / 2.0f, transform->direction);
		    }
		}	
};
