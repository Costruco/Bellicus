#pragma once

#include "SDL2/SDL.h"

#include <string>
#include "Components.hpp"
#include "TextureManager.hpp"

class SpriteComponent : public Component {
	private:
		TransformComponent * transform;
		TextureManager::TexturePtr texture;
		SDL_Rect src;
		SDL_FRect dst;
		SDL_FPoint texture_center;
		
		bool animated = false;
		int frames;
		int frameDelay;
	
	public:
		SpriteComponent(std::string path) {
			setTexture(path);
		}
		SpriteComponent(std::string path, int nFrames, int delay) {
			setTexture(path);
			animated = true;
			frames = nFrames;
			frameDelay = delay;
		}
		~SpriteComponent() {
		}
		
		void setTexture(std::string path) {
			texture = TextureManager::loadTexture(path);
		}
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
			src = {0,0,static_cast<int>(transform->width),static_cast<int>(transform->height)};
		}
		
		void update() override {
			if (animated) {
				src.x = src.w*static_cast<int>((SDL_GetTicks()/frameDelay)%frames);
			}
			texture_center = {(transform->width/2.0f+transform->center_offset.x)*transform->getScale(),
							  (transform->height/2.0f+transform->center_offset.y)*transform->getScale()};
			Vector2D pos = transform->getPosition();
			dst = {pos.x-texture_center.x,
				   pos.y-texture_center.y,
				   transform->width*transform->getScale(),
				   transform->height*transform->getScale()};
		}
		
		void draw() override {
			TextureManager::drawTexture(texture,&src,&dst,transform->getDirection(),&texture_center,SDL_FLIP_NONE);
		}
};
