#ifndef SPRITE_COMPONENT_HPP
#define SPRITE_COMPONENT_HPP

#include "SDL2/SDL.h"

#include "TextureManager.hpp"
#include "Components.hpp"

class SpriteComponent : public Component {
	private:
		PositionComponent * position;
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
			position = &entity->getComponent<PositionComponent>();
			SDL_QueryTexture(texture,NULL,NULL,&width,&height);
			src = {0,0,width,height};
			dst = {position->x(),position->y(),width,height};
		}
		void update() override {
			dst.x = position->x();
			dst.y = position->y();
		}
		void draw() override {
			TextureManager::drawTexture(texture,&src,&dst);
		}
};

#endif
