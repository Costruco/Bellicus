#include "TextureManager.hpp"

#include "SDL2/SDL_image.h"

#include "Game.hpp"

SDL_Texture * TextureManager::loadTexture(const char * fileName) {
	SDL_Texture * tex = IMG_LoadTexture(Game::ren,fileName);
	return tex;
}

void TextureManager::drawTexture(SDL_Texture * tex, SDL_Rect * src, SDL_Rect * dst, double angle, SDL_Point * center, SDL_RendererFlip flip) {
	SDL_RenderCopyEx(Game::ren,tex,src,dst,angle,center,flip);
}

void TextureManager::drawTexture(SDL_Texture * tex, SDL_Rect * src, SDL_Rect * dst) {
	SDL_RenderCopy(Game::ren,tex,src,dst);
}
