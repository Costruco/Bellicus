#include "TextureManager.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "Game.hpp"
#include <unordered_map>
#include <string>
#include <memory>

std::unordered_map<std::string,std::weak_ptr<SDL_Texture>> TextureManager::cache;

TextureManager::TexturePtr TextureManager::loadTexture(const std::string& fileName) {
	auto it = cache.find(fileName);
    if (it != cache.end()) {
        if (auto tex = it->second.lock()) {
            return tex;
        }
    }

    SDL_Texture* raw = IMG_LoadTexture(Game::ren, fileName.c_str());
    if (!raw) {
        SDL_Log("Failed to load texture: %s", IMG_GetError());
        return nullptr;
    }

    TextureManager::TexturePtr tex(raw, SDL_DestroyTexture);
    cache[fileName] = tex;
    return tex;
}

void TextureManager::drawTexture(TextureManager::TexturePtr tex, const SDL_Rect * src, const SDL_FRect * dst) {
	SDL_RenderCopyF(Game::ren,tex.get(),src,dst);
}

void TextureManager::drawTexture(TextureManager::TexturePtr tex, const SDL_Rect * src, const SDL_FRect * dst, double angle, SDL_FPoint * center, SDL_RendererFlip flip) {
	SDL_RenderCopyExF(Game::ren,tex.get(),src,dst,angle,center,flip);
}

void TextureManager::unload(const std::string& path) {
    cache.erase(path);
}

void TextureManager::clear() {
    cache.clear();
}
