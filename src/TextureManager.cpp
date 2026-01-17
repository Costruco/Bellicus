#include "TextureManager.hpp"
#include "Game.hpp"

SDL_Texture * TextureManager::loadTexture(const char * fileName) {
	SDL_Texture * tex = IMG_LoadTexture(Game::ren,fileName);
	return tex;
}

void TextureManager::drawTexture(SDL_Texture * tex, SDL_Rect * src, SDL_Rect * dst) {
	SDL_RenderCopy(Game::ren,tex,src,dst);
}
