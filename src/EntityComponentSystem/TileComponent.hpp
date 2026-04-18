#pragma once

#include "Components.hpp"
#include "SDL2/SDL.h"

class TileComponent : public Component {
	public:
		TransformComponent * transform;
		SpriteComponent * sprite;
		
		SDL_Rect tile;
		int tileID;
		const char * path;
		
		TileComponent(int x, int y, int w, int h, int id) {
			tile = {x,y,w,h};
			tileID = id;
			
			switch (tileID) {
				case 0:
					path = "../assets/textures/tiles/tile_map_1.png";
					break;
				case 1:
					path = "../assets/textures/tiles/tile_map_2.png";
					break;
				default:
					break;
			}
		}
		
		void init() override {
			entity->addComponent<TransformComponent>((float)tile.x,(float)tile.y,(float)tile.w,(float)tile.h);
			transform = &entity->getComponent<TransformComponent>();
			entity->addComponent<SpriteComponent>(path);
			sprite = &entity->getComponent<SpriteComponent>();
		}
};
