#include "TextureManager.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "Game.hpp"
#include "Vector2D.hpp"
#include <unordered_map>
#include <string>
#include <memory>

std::unordered_map<std::string,std::weak_ptr<SDL_Texture>> TextureManager::cache;

TextureManager::TexturePtr TextureManager::loadTexture(const std::string& fileName) {
    auto it = cache.find(fileName);
    if (it != cache.end())
        if (auto tex = it->second.lock())
            return tex;

    SDL_Texture* raw = IMG_LoadTexture(Game::ren, fileName.c_str());
    if (!raw) {
        SDL_Log("Failed to load texture: %s",IMG_GetError());
        return nullptr;
    }

    TextureManager::TexturePtr tex(raw,SDL_DestroyTexture);
    cache[fileName] = tex;
    return tex;
}

void TextureManager::drawTexture(TextureManager::TexturePtr tex, const SDL_Rect* src, const SDL_FRect* dst, 
                                double angle, SDL_FPoint* center, SDL_RendererFlip flip) {
    Vector2D localCenter = (center)?Vector2D(center->x,center->y):Vector2D(dst->w/2,dst->h/2);
    Vector2D worldCenter = localCenter+Vector2D(dst->x,dst->y);

    Vector2D screenCenter = Game::camera.worldToScreen(worldCenter);
    Vector2D screenSize = Game::camera.worldSizeToScreen(Vector2D(dst->w,dst->h));
    Vector2D scaledCenter = Game::camera.worldSizeToScreen(localCenter);

    SDL_FRect screenDst = {screenCenter.x-scaledCenter.x,
                           screenCenter.y-scaledCenter.y,
                           screenSize.x,screenSize.y};
    SDL_FPoint fscaledCenter = {scaledCenter.x,scaledCenter.y};

    SDL_RenderCopyExF(Game::ren,tex.get(),src,&screenDst,angle-Game::camera.angle,&fscaledCenter,flip);
}

void TextureManager::unload(const std::string& path) {
    cache.erase(path);
}

void TextureManager::clear() {
    cache.clear();
}