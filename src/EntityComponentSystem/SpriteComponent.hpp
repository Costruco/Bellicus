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
		SpriteComponent() = default;
		SpriteComponent(const char * path) {
			setTexture(path);
			src = {0,0,-1,-1};
		}
		SpriteComponent(const char * path, SDL_FPoint texture_center, bool fullTexture) {
			setTexture(path);
			if (fullTexture) {
				int width, height;
				SDL_QueryTexture(texture,NULL,NULL,&width,&height);
				src = {0,0,width,height};
			} else
				src = {0,0,-1,-1};
			this->texture_center = texture_center;
		}
		SpriteComponent(const char * path, bool fullTexture) {
			setTexture(path);
			if (fullTexture) {
				int width, height;
				SDL_QueryTexture(texture,NULL,NULL,&width,&height);
				src = {0,0,width,height};
				texture_center = {(float)width/2,(float)height/2};
			} else
				src = {0,0,-1,-1};
		}
		~SpriteComponent() {
			TextureManager::destroyTexture(texture);
		}
		
		void setTexture(const char * path) {
			texture = TextureManager::loadTexture(path);
		}
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
			if (src.w == -1) {
				src.w = transform->width;
				src.h = transform->height;
				texture_center = {transform->width/2,transform->height/2};
			} else {
				transform->width = src.w;
				transform->height = src.h;
			}
			
			dst = {(int)transform->position.x-texture_center.x,
				   (int)transform->position.y-texture_center.y,
				   transform->width*transform->scale,
				   transform->height*transform->scale};
		}
		
		void update() override {
			dst.x = (int)transform->position.x-texture_center.x;
			dst.y = (int)transform->position.y-texture_center.y;
		}
		
		void draw() override {
			TextureManager::drawTexture(texture,&src,&dst,transform->direction,nullptr,SDL_FLIP_NONE);
		}
};
