#ifndef SPRITE_COMPONENT_HPP
#define SPRITE_COMPONENT_HPP

#include "SDL2/SDL.h"

#include "Components.hpp"
#include "TextureManager.hpp"

class SpriteComponent : public Component {
	private:
		TransformComponent * transform;
		SDL_Texture * texture;
		SDL_Rect src,
				 dst;
	
	public:
		SpriteComponent() = default;
		SpriteComponent(const char * path) {
			setTexture(path);
		}
		
		void setTexture(const char * path) {
			texture = TextureManager::loadTexture(path);
		}
		
		void init() override {
			int width, height;
			transform = &entity->getComponent<TransformComponent>();
			SDL_QueryTexture(texture,NULL,NULL,&width,&height);
			src = {0,0,width,height};
			dst = {(int)transform->position.x,(int)transform->position.y,width,height};
		}
		void update() override {
			dst.x = (int)transform->position.x;
			dst.y = (int)transform->position.y;
		}
		void draw() override {
			TextureManager::drawTexture(texture,&src,&dst,transform->direction,nullptr,SDL_FLIP_NONE);
		}
};

#endif
