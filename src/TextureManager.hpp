#ifndef TEXTUREMANAGER_HPP
#define TEXTUREMANAGER_HPP

#include "Game.hpp"

class TextureManager {
	public:
		static SDL_Texture * LoadTexture(SDL_Renderer * ren, const char * fileName);
};

#endif
