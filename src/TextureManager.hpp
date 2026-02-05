#ifndef TEXTUREMANAGER_HPP
#define TEXTUREMANAGER_HPP

#include "SDL2/SDL.h"

class TextureManager {
	public:
		static SDL_Texture * loadTexture(const char * fileName);
		static void drawTexture(SDL_Texture * tex, SDL_Rect * src, SDL_Rect * dst, double angle, SDL_Point * center, SDL_RendererFlip flip);
		static void drawTexture(SDL_Texture * tex, SDL_Rect * src, SDL_Rect * dst);
		static void destroyTexture(SDL_Texture * tex);
};

#endif
