#pragma once

#include "Components.hpp"
#include "Game.hpp"
#include "Input.hpp"

class KeyboardController : public Component {
	public:
		TransformComponent * transform;
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
			const Uint8* keystate = Game::keystate;
	
			//direction
		    if (keystate[SDL_SCANCODE_A] && !keystate[SDL_SCANCODE_D])
		        transform->turnIntent = TurnDirection::LEFT;
		    else if (keystate[SDL_SCANCODE_D] && !keystate[SDL_SCANCODE_A])
		        transform->turnIntent = TurnDirection::RIGHT;
		    else
		        transform->turnIntent = TurnDirection::STRAIGHT;
		        
			//movement
		    if (keystate[SDL_SCANCODE_W] && !keystate[SDL_SCANCODE_S])
		        transform->moveIntent = MovementDirection::FORWARD;
		    else if (keystate[SDL_SCANCODE_S] && !keystate[SDL_SCANCODE_W])
		        transform->moveIntent = MovementDirection::BACKWARD;
		    else
		        transform->moveIntent = MovementDirection::STILL;
		}
};
