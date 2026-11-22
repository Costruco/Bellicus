#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ScreenAlignment.hpp"
#include "Utils/Vector2D.hpp"

class ResourceManager {
	private:
		static std::unordered_map<std::string,std::weak_ptr<SDL_Texture>> textureCache;
		static std::unordered_map<std::string,std::weak_ptr<TTF_Font>> fontCache;
		static std::unordered_map<std::string,std::weak_ptr<Mix_Chunk>> soundCache;
	public:
		using TexturePtr = std::shared_ptr<SDL_Texture>;
		using FontPtr = std::shared_ptr<TTF_Font>;
		using SoundPtr = std::shared_ptr<Mix_Chunk>;
	
		static TexturePtr loadTexture(const std::string& fileName, bool isLinear = false);
		static FontPtr loadFont(const std::string& fileName, int fontSize);
		static SoundPtr loadSound(const std::string& fileName);
		static void drawTexture(const TexturePtr& tex, const SDL_Rect * src, const SDL_FRect * dst, 
			double angle = 0, SDL_FPoint * center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);
		static void playSound(const SoundPtr& sound, const Vector2D& position, int loops = 0, int channel = -1);
		static void drawThickRectangle(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 thickness, SDL_Color color);
		static void drawThickRectangle(const SDL_FRect * dst, Uint8 thickness, SDL_Color color);
		static void drawMessageBox(const FontPtr& font, const SDL_FRect * dst, ScreenAlignment alignment, 
			int padding, int lineSpacing, const std::vector<std::string>& lines, 
			SDL_Color text, SDL_Color border, SDL_Color background, float scale = 1.0f);
		static void unloadTextTexture(const std::string& key);
		static void unloadTexture(const std::string& path);
		static void unloadFont(const std::string& path);
		static void unloadSound(const std::string& path);
		static void clear();
};