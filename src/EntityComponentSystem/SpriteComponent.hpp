#pragma once

#include "SDL2/SDL.h"

#include "Components.hpp"
#include "TextureManager.hpp"

class SpriteComponent : public Component {
	private:
		TransformComponent * transform;
		SDL_Texture * texture;
		SDL_Rect src;
		SDL_FRect dst;
		SDL_FPoint texture_center;
	
	public:
		SpriteComponent(const char * path) {
			setTexture(path);
			src.x = 0;
			src.y = 0;
			SDL_QueryTexture(texture,NULL,NULL,&src.w,&src.h);
		}
		SpriteComponent(const char * path, const SDL_Rect& src) {
			setTexture(path);
			this->src = src;
			texture_center = {(float)src.w/2,(float)src.h/2};
		}
		~SpriteComponent() {
			TextureManager::destroyTexture(texture);
		}
		
		void setTexture(const char * path) {
			texture = TextureManager::loadTexture(path);
		}
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
			texture_center = {transform->width/2+transform->center_offset.x,
							  transform->height/2+transform->center_offset.y};
			dst = {transform->position.x-texture_center.x*transform->scale,
				   transform->position.y-texture_center.y*transform->scale,
				   transform->width*transform->scale,
				   transform->height*transform->scale};
		}
		
		void draw() override {
			TextureManager::drawTexture(texture,&src,&dst,transform->direction,&texture_center,SDL_FLIP_NONE);
		}
};
