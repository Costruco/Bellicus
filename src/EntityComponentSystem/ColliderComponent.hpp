#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include <string>
#include "Vector2D.hpp"
#include "Components.hpp"
#include "Game.hpp"

class ColliderComponent : public Component {
	public:
		SDL_FRect collider;
		std::string tag;
		TransformComponent * transform;
		
		ColliderComponent(std::string tag) {
			this->tag = tag;
		}
		
		void init() override {
			if (!entity->hasComponent<TransformComponent>())
				entity->addComponent<TransformComponent>();
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
			collider = {transform->position.x-(transform->width/2+transform->center_offset.x)*transform->scale,
						transform->position.y-(transform->height/2+transform->center_offset.y)*transform->scale,
						transform->width*transform->scale,
						transform->height*transform->scale};
		}
		
		void draw() override {
			float hw = transform->width*0.5f*transform->scale;
			float hh = transform->height*0.5f*transform->scale;
			
			Vector2D center = transform->position-transform->center_offset*transform->scale;
			
			Vector2D topLeft = center+Vector2D{-hw,-hh};
			Vector2D topRight = center+Vector2D{ hw,-hh};
			Vector2D bottomRight = center+Vector2D{hw,hh};
			Vector2D bottomLeft = center+Vector2D{-hw,hh};
			
			topLeft = topLeft.rotate(center,transform->direction);
			topRight = topRight.rotate(center,transform->direction);
			bottomRight = bottomRight.rotate(center,transform->direction);
			bottomLeft = bottomLeft.rotate(center,transform->direction);
			
			lineRGBA(Game::ren,topLeft.x,topLeft.y,topRight.x,topRight.y,255,0,0,255);
			lineRGBA(Game::ren,topRight.x,topRight.y,bottomRight.x,bottomRight.y,255,0,0,255);
			lineRGBA(Game::ren,bottomRight.x,bottomRight.y,bottomLeft.x,bottomLeft.y,255,0,0,255);
			lineRGBA(Game::ren,bottomLeft.x,bottomLeft.y,topLeft.x,topLeft.y,255,0,0,255);
		}
};
