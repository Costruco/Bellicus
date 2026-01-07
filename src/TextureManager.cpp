#include "TextureManager.hpp"

SDL_Texture * TextureManager::LoadTexture(SDL_Renderer * ren, const char * fileName) {
	SDL_Texture * tex = IMG_LoadTexture(ren,fileName);
	return tex;
}
