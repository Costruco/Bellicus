#include "TextureManager.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "Game.hpp"

SDL_Texture * TextureManager::loadTexture(const char * fileName) {
	SDL_Texture * tex = IMG_LoadTexture(Game::ren,fileName);
	return tex;
}

void TextureManager::drawTexture(SDL_Texture * tex, SDL_Rect * src, SDL_FRect * dst) {
	SDL_RenderCopyF(Game::ren,tex,src,dst);
}

void TextureManager::drawTexture(SDL_Texture * tex, SDL_Rect * src, SDL_FRect * dst, double angle, SDL_FPoint * center, SDL_RendererFlip flip) {
	SDL_RenderCopyExF(Game::ren,tex,src,dst,angle,center,flip);
}

void TextureManager::destroyTexture(SDL_Texture * tex) {
	SDL_DestroyTexture(tex);
}
