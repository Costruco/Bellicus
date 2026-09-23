#pragma once

#include "Components.hpp"
#include "SDL2/SDL.h"
#include <string>
#include "TextureManager.hpp"

class TileComponent : public Component {
	public:
		TextureManager::TexturePtr texture;
		SDL_Rect src;
		SDL_FRect dst;

		TileComponent() = default;
		TileComponent(int srcx, int srcy, int x, int y, const std::string& path) {
			texture = TextureManager::loadTexture(path);
			src.x = srcx;
			src.y = srcy;
			src.w = src.h = TILE_SIZE;

			dst.x = x;
			dst.y = y;
			dst.w = dst.h = TILE_SIZE;
		}
		~TileComponent() {
		}

		void draw() override {
			TextureManager::drawTexture(texture,&src,&dst);
		}
};