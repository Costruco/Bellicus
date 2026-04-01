#pragma once

#include "SDL2/SDL.h"

#include "Components.hpp"
#include "TextureManager.hpp"

class SpriteComponent : public Component {
	private:
		TransformComponent * transform;
		SDL_Texture * texture;
		SDL_Rect src,
				 dst;
		SDL_Point texture_center;
	
	public:
		SpriteComponent() = default;
		SpriteComponent(const char * path) {
			setTexture(path);
			int width, height;
			SDL_QueryTexture(texture,NULL,NULL,&width,&height);
			src = {0,0,width,height};
			texture_center = {width/2,height/2};
		}
		SpriteComponent(const char * path, SDL_Point texture_center) {
			setTexture(path);
			int width, height;
			SDL_QueryTexture(texture,NULL,NULL,&width,&height);
			src = {0,0,width,height};
			this->texture_center = texture_center;
		}
		~SpriteComponent() {
			TextureManager::destroyTexture(texture);
		}
		
		void setTexture(const char * path) {
			texture = TextureManager::loadTexture(path);
		}
		
		void init() override {
			int width, height;
			SDL_QueryTexture(texture,NULL,NULL,&width,&height);
			transform = &entity->getComponent<TransformComponent>();
			dst = {(int)transform->position.x-texture_center.x,(int)transform->position.y-texture_center.y,width,height};
		}
		
		void update() override {
			dst.x = (int)transform->position.x-texture_center.x;
			dst.y = (int)transform->position.y-texture_center.y;
		}
		
		void draw() override {
			TextureManager::drawTexture(texture,&src,&dst,transform->direction,nullptr,SDL_FLIP_NONE);
		}
};
