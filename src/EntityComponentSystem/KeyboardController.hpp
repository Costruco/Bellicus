#ifndef KEYBOARD_CONTROLLER_HPP
#define KEYBOARD_CONTROLLER_HPP

#include "Components.hpp"
#include "Game.hpp"

class KeyboardController : public Component {
	public:
		TransformComponent * transform;
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
			transform->direction = Vector2D();
		}
		
		void update() override {
			if (Game::evt.type == SDL_KEYDOWN) {
				switch (Game::evt.key.keysym.sym) {
					case SDLK_w:
						transform->direction.y = -1;
						break;
					case SDLK_a:
						transform->direction.x = -1;
						break;
					case SDLK_s:
						transform->direction.y = +1;
						break;
					case SDLK_d:
						transform->direction.x = +1;
						break;
					default:
						break;
				}
			}
			if (Game::evt.type == SDL_KEYUP) {
				switch (Game::evt.key.keysym.sym) {
					case SDLK_w:
						transform->direction.y = 0;
						break;
					case SDLK_a:
						transform->direction.x = 0;
						break;
					case SDLK_s:
						transform->direction.y = 0;
						break;
					case SDLK_d:
						transform->direction.x = 0;
						break;
					default:
						break;
				}
			}
		}
};

#endif
