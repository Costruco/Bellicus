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
#include "Camera.hpp"
#include "ResourceManager.hpp"
#include "ScreenAlignment.hpp"
#include "Utils/Vector2D.hpp"

std::unordered_map<std::string,std::weak_ptr<SDL_Texture>> ResourceManager::textureCache;
std::unordered_map<std::string,std::weak_ptr<TTF_Font>> ResourceManager::fontCache;
std::unordered_map<std::string,std::weak_ptr<Mix_Chunk>> ResourceManager::soundCache;

ResourceManager::TexturePtr ResourceManager::loadTexture(const std::string& fileName, bool isLinear) {
    auto it = textureCache.find(fileName);
    if (it != textureCache.end())
        if (auto tex = it->second.lock())
            return tex;

    if (isLinear)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    SDL_Texture * raw = IMG_LoadTexture(Game::ren, fileName.c_str());
    if (isLinear)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

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

namespace {
    struct CachedTextTexture {
        ResourceManager::TexturePtr texture;
        int w = 0;
        int h = 0;
    };
    std::unordered_map<std::string, CachedTextTexture> textTextureCache;
}

void ResourceManager::drawTexture(const TexturePtr& tex, const SDL_Rect* src, const SDL_FRect* dst, 
    double angle, SDL_FPoint* center, SDL_RendererFlip flip) {
    Vector2D localCenter = (center)?Vector2D(center->x,center->y):Vector2D(dst->w/2,dst->h/2);
    Vector2D worldCenter = localCenter+Vector2D(dst->x,dst->y);
    Vector2D screenSize = Game::camera.worldSizeToScreen(Vector2D(dst->w,dst->h));
    float radius = (screenSize.x+screenSize.y)*0.5f;

    float dx = std::abs(worldCenter.x-Game::camera.focus.x)*Game::camera.scale.x;
    float dy = std::abs(worldCenter.y-Game::camera.focus.y)*Game::camera.scale.y;
    float maxExtent = (Game::WINDOW_WIDTH+Game::WINDOW_HEIGHT)*0.5f+radius*2.0f;
    if (dx > maxExtent || dy > maxExtent) {
        return;
    }

    Vector2D screenCenter = Game::camera.worldToScreen(worldCenter);
    if (screenCenter.x+radius < 0 || screenCenter.x-radius > Game::WINDOW_WIDTH ||
        screenCenter.y+radius < 0 || screenCenter.y-radius > Game::WINDOW_HEIGHT) {
        return;
    }

    Vector2D scaledCenter = Game::camera.worldSizeToScreen(localCenter);
    SDL_FRect screenDst = {screenCenter.x-scaledCenter.x,
                           screenCenter.y-scaledCenter.y,
                           screenSize.x,screenSize.y};
    SDL_FPoint fscaledCenter = {scaledCenter.x,scaledCenter.y};

    SDL_RenderCopyExF(Game::ren,tex.get(),src,&screenDst,angle-Game::camera.angle,&fscaledCenter,flip);
}

void ResourceManager::playSound(const SoundPtr& sound, const Vector2D& position, int loops, int channel) {
    if (!sound.get())
        return;
    float distance = position.getDistance(Game::camera.focus)/Game::camera.maxAudioDistance;
    if (distance > 1.0f)
        return;
    int playedChannel = Mix_PlayChannel(channel, sound.get(), loops);
    if (playedChannel != -1) {
        Mix_SetDistance(playedChannel, static_cast<Uint8>(distance*255));
    }
}

void ResourceManager::drawThickRectangle(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 thickness, SDL_Color color) {
    if (thickness == 0)
        return;
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);

    boxRGBA(Game::ren, x1, y1, x2, y1 + thickness - 1, color.r, color.g, color.b, color.a);
    boxRGBA(Game::ren, x1, y2 - thickness + 1, x2, y2, color.r, color.g, color.b, color.a);
    if (y2 - y1 >= 2 * thickness) {
        boxRGBA(Game::ren, x1, y1 + thickness, x1 + thickness - 1, y2 - thickness, color.r, color.g, color.b, color.a);
        boxRGBA(Game::ren, x2 - thickness + 1, y1 + thickness, x2, y2 - thickness, color.r, color.g, color.b, color.a);
    }
}

void ResourceManager::drawThickRectangle(const SDL_FRect * dst, Uint8 thickness, SDL_Color color) {
    drawThickRectangle(
        static_cast<Sint16>(dst->x),
        static_cast<Sint16>(dst->y),
        static_cast<Sint16>(dst->x + dst->w),
        static_cast<Sint16>(dst->y + dst->h),
        thickness, color
    );
}

void ResourceManager::drawMessageBox(const FontPtr& font, const SDL_FRect * dst, ScreenAlignment alignment, 
    int padding, int lineSpacing, const std::vector<std::string>& lines, 
    SDL_Color text, SDL_Color border, SDL_Color background, float scale) {
    
    int outW, outH;
    SDL_GetRendererOutputSize(Game::ren, &outW, &outH);
    float scaleX, scaleY;
    SDL_RenderGetScale(Game::ren, &scaleX, &scaleY);
    float screenW = outW / scaleX;
    float screenH = outH / scaleY;

    float scaledPadding = padding * scale;
    float scaledLineSpacing = lineSpacing * scale;
    int borderThickness = std::max(1, static_cast<int>(scale));

    SDL_FRect boxDst = *dst;
    boxDst.w = boxDst.w * scale + 2.0f * scaledPadding;
    boxDst.h = boxDst.h * scale + 2.0f * scaledPadding + scaledLineSpacing * (lines.size() - 1);

    ScreenAlignment h = alignment & ScreenAlignment::HORIZONTAL;
    ScreenAlignment v = alignment & ScreenAlignment::VERTICAL;
    switch (h) {
        case ScreenAlignment::LEFT:
        default:
            boxDst.x += borderThickness;
            break;
        case ScreenAlignment::HCENTER:
            boxDst.x += (screenW - boxDst.w) / 2.0f;
            break;
        case ScreenAlignment::RIGHT:
            boxDst.x += screenW - boxDst.w - borderThickness - 1;
            break;
    }
    switch (v) {
        case ScreenAlignment::TOP:
        default:
            boxDst.y += borderThickness;
            break;
        case ScreenAlignment::VCENTER:
            boxDst.y += (screenH - boxDst.h) / 2.0f;
            break;
        case ScreenAlignment::BOTTOM:
            boxDst.y += screenH - boxDst.h - borderThickness - 1;
            break;
    }

    boxRGBA(Game::ren,boxDst.x,boxDst.y,boxDst.x+boxDst.w,boxDst.y+boxDst.h,
        background.r,background.g,background.b,background.a);
    drawThickRectangle(
        boxDst.x - borderThickness,
        boxDst.y - borderThickness,
        boxDst.x + boxDst.w + borderThickness,
        boxDst.y + boxDst.h + borderThickness,
        borderThickness, border
    );

    if (textTextureCache.size() > 512) {
        textTextureCache.clear();
    }

    for (size_t i = 0; i < lines.size(); i++) {
        const std::string& line = lines[i];
        auto centerPos = line.find("/center");
        auto rightPos = line.find("/right");
        bool c = (centerPos != std::string::npos);
        bool r = (rightPos != std::string::npos);
        
        std::string left = line.substr(0,(c?centerPos:rightPos));
        std::string center = (c?line.substr(centerPos+7,(r?rightPos-(centerPos+7):std::string::npos)):"");
        std::string right = (r?line.substr(rightPos+6):"");

        auto drawText = [&](const std::string& s, int alignment) {
            if (s.empty())
                return;
            std::string key;
            key.reserve(s.size() + 16);
            key.append(reinterpret_cast<const char*>(&font), sizeof(TTF_Font*));
            key.push_back(static_cast<char>(text.r));
            key.push_back(static_cast<char>(text.g));
            key.push_back(static_cast<char>(text.b));
            key.append(s);

            auto it = textTextureCache.find(key);
            if (it == textTextureCache.end()) {
                SDL_Surface* surface = TTF_RenderUTF8_Solid(font.get(),s.c_str(),text);
                if (!surface)
                    return;
                SDL_Texture* rawTex = SDL_CreateTextureFromSurface(Game::ren,surface);
                int w = surface->w;
                int h = surface->h;
                SDL_FreeSurface(surface);
                if (!rawTex)
                    return;
                it = textTextureCache.emplace(std::move(key), CachedTextTexture{
                    TexturePtr(rawTex, SDL_DestroyTexture), w, h
                }).first;
            }

            const auto& cached = it->second;
            float texW = cached.w * scale;
            float texH = cached.h * scale;
            float x = scaledPadding+boxDst.x+(boxDst.w-scaledPadding*2.0f-texW)*alignment/2.0f;
            float lineHeight = (boxDst.h-scaledPadding*2.0f+scaledLineSpacing)/lines.size()-scaledLineSpacing;
            float y = scaledPadding+boxDst.y+(lineHeight+scaledLineSpacing)*i+(lineHeight-texH)/2.0f;
            SDL_FRect textDst = {x,y,texW,texH};
            SDL_SetTextureAlphaMod(cached.texture.get(),text.a);
            SDL_RenderCopyF(Game::ren,cached.texture.get(),nullptr,&textDst);
        };
        drawText(left,0);
        drawText(center,1);
        drawText(right,2);
    }
}

void ResourceManager::unloadTextTexture(const std::string& key) {
    textTextureCache.erase(key);
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
    textTextureCache.clear();
    textureCache.clear();
    fontCache.clear();
    soundCache.clear();
}