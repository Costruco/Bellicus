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
			if (Game::evt.type == SDL_KEYDOWN) {
				switch (Game::evt.key.keysym.sym) {
					case SDLK_w:
						transform->moveIntent += 1;
						break;
					case SDLK_a:
						transform->turnIntent -= 1;
						break;
					case SDLK_s:
						transform->moveIntent -= 1;
						break;
					case SDLK_d:
						transform->turnIntent += 1;
						break;
					default:
						break;
				}
			}
			if (Game::evt.type == SDL_KEYUP) {
				switch (Game::evt.key.keysym.sym) {
					case SDLK_w:
						transform->moveIntent -= 1;
						break;
					case SDLK_a:
						transform->turnIntent += 1;
						break;
					case SDLK_s:
						transform->moveIntent += 1;
						break;
					case SDLK_d:
						transform->turnIntent -= 1;
						break;
					default:
						break;
				}
			}
		}
};
