#ifndef VEHICLE_MOVEMENT_COMPONENT_HPP
#define VEHICLE_MOVEMENT_COMPONENT_HPP

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

class TankMovementComponent : public Component {
	public:
		Map& map;
		TransformComponent * transform;
		float speed;
		float turningSpeed;
		int enginePower; 
		int mass;
		int brakePower;
		float rollingDrag;
		
		//gearSystem
		Uint32 gearChangeStart;
		bool increasing;
		bool decreasing;
		
		int nGears;
		std::vector<SDL_Point> gearsMaxSpeed;
		int neutralGear;
		int currentGear;
			   
		TankMovementComponent(float speed, float turningSpeed, int enginePower, int mass, int brakePower, float rollingDrag, 
		Map& m, int nGears, std::vector<SDL_Point> maxSpeedArray) : map(m), gearsMaxSpeed(maxSpeedArray) {
			this->speed = speed;
			this->turningSpeed = turningSpeed;
			this->enginePower = enginePower;
			this->mass = mass;
			this->brakePower = brakePower;
			this->rollingDrag = rollingDrag;
			
			//gearSystem
			this->nGears = nGears;
			for (int i = 0; i < nGears; i++) {
				if (gearsMaxSpeed[i].x == 0 && gearsMaxSpeed[i].y == 0) {
					currentGear = neutralGear = i;
					break;
				}
		    }
		    increasing = false;
		    decreasing = false;
		}
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
			float groundDrag = map.getTileDrag(static_cast<int>(transform->position.x)/100,static_cast<int>(transform->position.y)/100);
			float dt = FrameManager::getDeltaTime();
			//std::cout << groundDrag << std::endl;
			
			//atualiza a currentGear do tanque com base na currentGear atual, speed e controle
			if (currentGear == neutralGear) {
				if (aproxEqual(speed,gearsMaxSpeed[currentGear].y) && increasing == false && transform->moveIntent == MovementDirection::FORWARD) {
					gearChangeStart = SDL_GetTicks();
					increasing = true;
				} else if (aproxEqual(speed,gearsMaxSpeed[currentGear].x) && decreasing == false && transform->moveIntent == MovementDirection::BACKWARD) {
					gearChangeStart = SDL_GetTicks();
					decreasing = true;
				}
			} else {
				if (currentGear != nGears-1 && aproxEqual(speed,gearsMaxSpeed[currentGear].y) && increasing == false && transform->moveIntent == MovementDirection::FORWARD) {
					gearChangeStart = SDL_GetTicks();
					increasing = true;
				} else if (currentGear > neutralGear && aproxEqual(speed,gearsMaxSpeed[currentGear].x) && decreasing == false && transform->moveIntent == MovementDirection::BACKWARD) {
					currentGear = MAX(currentGear-1,neutralGear);
				} else if (currentGear > neutralGear && aproxEqual(speed,gearsMaxSpeed[currentGear].x) && decreasing == false && transform->moveIntent == MovementDirection::STILL) {
					gearChangeStart = SDL_GetTicks();
					decreasing = true;
				}
			}
			
			if (increasing == true && SDL_GetTicks()-gearChangeStart >= 250) {
				currentGear = MIN(currentGear+1,nGears-1);
				increasing = false;
			} else if (decreasing == true && SDL_GetTicks()-gearChangeStart >= 250) {
				currentGear = MAX(currentGear-1,0);
				decreasing = false;
			}
			
			//movement
			int troca = (increasing == true || decreasing == true);
			double TURNING = 0.035*(transform->turnIntent != TurnDirection::STRAIGHT);
			if (speed < 0) {
				if (transform->moveIntent == MovementDirection::STILL)
					speed = MIN(gearsMaxSpeed[currentGear].y,speed+desacc(brakePower,mass)*32.5*dt);
				else if (transform->moveIntent == MovementDirection::FORWARD)
					speed = MIN(gearsMaxSpeed[currentGear].y,speed+desacc(brakePower,mass)*32.5*dt);
				else if (transform->moveIntent == MovementDirection::BACKWARD && !troca)
					speed = MAX(gearsMaxSpeed[currentGear].x,speed+acc(speed,mass,enginePower,groundDrag,rollingDrag+TURNING)*dt);
			} else if (speed == 0) {
				if (transform->moveIntent == MovementDirection::FORWARD && !troca)
					speed = MAX(gearsMaxSpeed[currentGear].x,speed+acc(speed,mass,enginePower,groundDrag,rollingDrag+TURNING)*dt);
				else if (transform->moveIntent == MovementDirection::BACKWARD && !troca)
					speed = MIN(gearsMaxSpeed[currentGear].y,speed-acc(speed,mass,enginePower,groundDrag,rollingDrag+TURNING)*dt);
			} else if (speed > 0){
				if (transform->moveIntent == MovementDirection::STILL)
					speed = MAX(gearsMaxSpeed[currentGear].x,speed-desacc(brakePower,mass)*32.5*dt);
				else if (transform->moveIntent == MovementDirection::FORWARD && !troca)
					speed = MIN(gearsMaxSpeed[currentGear].y,speed+acc(speed,mass,enginePower,groundDrag,rollingDrag+TURNING)*dt);
				else if (transform->moveIntent == MovementDirection::BACKWARD)
					speed = MAX(gearsMaxSpeed[currentGear].x,speed-desacc(brakePower,mass)*32.5*dt);
			}
			//movement
			transform->position += Vector2D::fromPolar(speed,transform->direction)*dt;
		
			//direction
			switch (transform->turnIntent) {
				case TurnDirection::STRAIGHT:
					break;
				case TurnDirection::LEFT:
					if (transform->moveIntent != MovementDirection::BACKWARD) {
						transform->direction += turningSpeed*dt;
						break;
					} else {
						transform->direction -= turningSpeed*dt;
						break;
					}
				case TurnDirection::RIGHT:
					if (transform->moveIntent != MovementDirection::BACKWARD) {
						transform->direction -= turningSpeed*dt;
						break;
					} else {
						transform->direction += turningSpeed*dt;
						break;
					}
			}
			clockLimit(transform->direction,0.0f,360.0f);
			
		}	
};

#endif
