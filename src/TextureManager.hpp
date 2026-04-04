#pragma once

#include "SDL2/SDL.h"

class TextureManager {
	public:
		static SDL_Texture * loadTexture(const char * fileName);
		static void drawTexture(SDL_Texture * tex, SDL_Rect * src, SDL_FRect * dst);
		static void drawTexture(SDL_Texture * tex, SDL_Rect * src, SDL_FRect * dst, double angle, SDL_FPoint * center, SDL_RendererFlip flip);
		static void destroyTexture(SDL_Texture * tex);
};
