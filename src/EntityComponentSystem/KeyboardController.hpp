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
			transform->turnIntent = TurnDirection::STRAIGHT;
			if (keystate[SDL_SCANCODE_A])
				transform->turnIntent = transform->turnIntent | TurnDirection::LEFT;
			if (keystate[SDL_SCANCODE_D])
				transform->turnIntent = transform->turnIntent | TurnDirection::RIGHT;
		        
			//movement
			transform->moveIntent = MovementDirection::STILL;
			if (keystate[SDL_SCANCODE_S])
				transform->moveIntent = transform->moveIntent | MovementDirection::BACKWARD;
			if (keystate[SDL_SCANCODE_W])
				transform->moveIntent = transform->moveIntent | MovementDirection::FORWARD;
		}
};