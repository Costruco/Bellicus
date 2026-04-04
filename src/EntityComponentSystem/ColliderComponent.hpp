#pragma once

#include "SDL2/SDL.h"
#include <string>
#include "Components.hpp"

class ColliderComponent : public Component {
	public:
		SDL_FRect collider;
		std::string tag;
		
		TransformComponent * transform;
		
		ColliderComponent(std::string tag) {
			this->tag = tag;
		}
		
		void init() override {
			if (!entity->hasComponent<TransformComponent>()) {
				entity->addComponent<TransformComponent>();
			}
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
			collider = {transform->position.x,
						transform->position.y,
						transform->width*transform->scale,
						transform->height*transform->scale};
		}
};
