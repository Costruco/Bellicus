#pragma once

#include "Components.hpp"
#include "Game.hpp"
#include "Input.hpp"
#include "MovementComponent.hpp"

class KeyboardController : public Component {
	public:
		MovementComponent * movement;
		
		void init() override {
			movement = &entity->getComponent<MovementComponent>();
		}
		
		void update() override {
			const Uint8* keystate = Game::keystate;
	
			//direction
			movement->turnIntent = TurnDirection::STRAIGHT;
			if (keystate[SDL_SCANCODE_A])
				movement->turnIntent = movement->turnIntent | TurnDirection::LEFT;
			if (keystate[SDL_SCANCODE_D])
				movement->turnIntent = movement->turnIntent | TurnDirection::RIGHT;
		        
			//movement
			movement->moveIntent = MovementDirection::STILL;
			if (keystate[SDL_SCANCODE_S])
				movement->moveIntent = movement->moveIntent | MovementDirection::BACKWARD;
			if (keystate[SDL_SCANCODE_W])
				movement->moveIntent = movement->moveIntent | MovementDirection::FORWARD;
		}
};
