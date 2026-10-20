#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Utils/Collision.hpp"
#include "Game.hpp"
#include "ResourceManager.hpp"
#include "ScreenAlignment.hpp"
#include "Utils/Vector2D.hpp"

std::unordered_map<std::string,std::weak_ptr<SDL_Texture>> ResourceManager::textureCache;
std::unordered_map<std::string,std::weak_ptr<TTF_Font>> ResourceManager::fontCache;
std::unordered_map<std::string,std::weak_ptr<Mix_Chunk>> ResourceManager::soundCache;

ResourceManager::TexturePtr ResourceManager::loadTexture(const std::string& fileName) {
    auto it = textureCache.find(fileName);
    if (it != textureCache.end())
        if (auto tex = it->second.lock())
            return tex;

    SDL_Texture* raw = IMG_LoadTexture(Game::ren, fileName.c_str());
    if (!raw) {
        SDL_Log("Failed to load texture: %s",IMG_GetError());
        return nullptr;
    }

    ResourceManager::TexturePtr tex(raw,SDL_DestroyTexture);
    textureCache[fileName] = tex;
    return tex;
}

ResourceManager::FontPtr ResourceManager::loadFont(const std::string& fileName, int fontSize) {
    auto it = fontCache.find(fileName);
    if (it != fontCache.end())
        if (auto font = it->second.lock())
            return font;

    TTF_Font* raw = TTF_OpenFont(fileName.c_str(), fontSize);
    if (!raw) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return nullptr;
    }

    ResourceManager::FontPtr font(raw, TTF_CloseFont);
    fontCache[fileName] = font;
    return font;
}

ResourceManager::SoundPtr ResourceManager::loadSound(const std::string& fileName) {
    auto it = soundCache.find(fileName);
    if (it != soundCache.end())
        if (auto sound = it->second.lock())
            return sound;

    Mix_Chunk* raw = Mix_LoadWAV(fileName.c_str());
    if (!raw) {
        SDL_Log("Failed to load sound: %s", Mix_GetError());
        return nullptr;
    }

    ResourceManager::SoundPtr sound(raw, Mix_FreeChunk);
    soundCache[fileName] = sound;
    return sound;
}

void ResourceManager::drawTexture(const TexturePtr& tex, const SDL_Rect* src, const SDL_FRect* dst, 
    double angle, SDL_FPoint* center, SDL_RendererFlip flip) {
    Vector2D localCenter = (center)?Vector2D(center->x,center->y):Vector2D(dst->w/2,dst->h/2);
    Vector2D worldCenter = localCenter+Vector2D(dst->x,dst->y);

    Vector2D screenCenter = Game::camera.worldToScreen(worldCenter);
    Vector2D screenSize = Game::camera.worldSizeToScreen(Vector2D(dst->w,dst->h));
    Vector2D scaledCenter = Game::camera.worldSizeToScreen(localCenter);

    SDL_FRect screenDst = {screenCenter.x-scaledCenter.x,
                           screenCenter.y-scaledCenter.y,
                           screenSize.x,screenSize.y};
    float radius = (screenSize.x+screenSize.y)*0.5f;
    if (screenCenter.x+radius < 0 || screenCenter.x-radius > Game::WINDOW_WIDTH ||
        screenCenter.y+radius < 0 || screenCenter.y-radius > Game::WINDOW_HEIGHT) {
        return;
    }
    SDL_FPoint fscaledCenter = {scaledCenter.x,scaledCenter.y};

    SDL_RenderCopyExF(Game::ren,tex.get(),src,&screenDst,angle-Game::camera.angle,&fscaledCenter,flip);
}

void ResourceManager::drawMessageBox(const FontPtr& font, const SDL_FRect * dst, ScreenAlignment alignment, 
    int padding, int lineSpacing, const std::vector<std::string>& lines, 
    SDL_Color text, SDL_Color border, SDL_Color background) {
    ScreenAlignment h = alignment & ScreenAlignment::HORIZONTAL;
    ScreenAlignment v = alignment & ScreenAlignment::VERTICAL;
    SDL_FRect boxDst = *dst;
    boxDst.x += 1;
    boxDst.y += 1;
    boxDst.w += 2*padding;
    boxDst.h += 2*padding+lineSpacing*(lines.size()-1);
    switch (h) {
        case ScreenAlignment::LEFT:
            break;
        case ScreenAlignment::HCENTER:
            boxDst.x = boxDst.x+Game::WINDOW_WIDTH/2-boxDst.w/2;
            break;
        case ScreenAlignment::RIGHT:
            boxDst.x = boxDst.x+Game::WINDOW_WIDTH-boxDst.w;
            break;
    }
    switch (v) {
        case ScreenAlignment::TOP:
            break;
        case ScreenAlignment::VCENTER:
            boxDst.y = boxDst.y+Game::WINDOW_HEIGHT/2-boxDst.h/2;
            break;
        case ScreenAlignment::BOTTOM:
            boxDst.y = boxDst.y+Game::WINDOW_HEIGHT-boxDst.h;
            break;
    }
    boxRGBA(Game::ren,boxDst.x,boxDst.y,boxDst.x+boxDst.w,boxDst.y+boxDst.h,
        background.r,background.g,background.b,background.a);
    rectangleRGBA(Game::ren,boxDst.x-1,boxDst.y-1,boxDst.x+boxDst.w+1,boxDst.y+boxDst.h+1,
        border.r,border.g,border.b,border.a);
    for (size_t i = 0; i < lines.size(); i++) {
        const std::string& line = lines[i];
        auto centerPos = line.find("/center");
        auto rightPos = line.find("/right");
        bool c = (centerPos != std::string::npos);
        bool r = (rightPos != std::string::npos);
        std::string left = line.substr(0,(c?centerPos:rightPos));
        std::string center = (c?line.substr(centerPos+7,(r?rightPos-(centerPos+7):r)):"");
        std::string right = (r?line.substr(rightPos+6):"");
        auto drawText = [&](const std::string& s, int alignment) {
            if (s.empty())
                return;
            SDL_Surface* surface = TTF_RenderUTF8_Solid(font.get(),s.c_str(),text);
            if (!surface)
                return;
            SDL_Texture* texture = SDL_CreateTextureFromSurface(Game::ren,surface);
            float x = padding+boxDst.x+(boxDst.w-padding*2.0f-surface->w)*alignment/2.0f;
            float lineHeight = (boxDst.h-padding*2+lineSpacing)/lines.size()-lineSpacing;
            float y = padding+boxDst.y+(lineHeight+lineSpacing)*i+(lineHeight-surface->h)/2.0f;
            SDL_FRect textDst = {x,y,static_cast<float>(surface->w),static_cast<float>(surface->h)};
            SDL_FreeSurface(surface);
            if (texture) {
                SDL_SetTextureAlphaMod(texture,text.a);
                SDL_RenderCopyF(Game::ren,texture,nullptr,&textDst);
                SDL_DestroyTexture(texture);
            }
        };
        drawText(left,0);
        drawText(center,1);
        drawText(right,2);
    }
}

void ResourceManager::unloadTexture(const std::string& path) {
    textureCache.erase(path);
}

void ResourceManager::unloadFont(const std::string& path) {
    fontCache.erase(path);
}

void ResourceManager::unloadSound(const std::string& path) {
    soundCache.erase(path);
}

void ResourceManager::clear() {
    textureCache.clear();
    fontCache.clear();
    soundCache.clear();
}