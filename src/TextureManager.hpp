#pragma once

#include "SDL2/SDL.h"
#include <unordered_map>
#include <string>
#include <memory>

class TextureManager {
	private:
		static std::unordered_map<std::string,std::weak_ptr<SDL_Texture>> cache;
	public:
		using TexturePtr = std::shared_ptr<SDL_Texture>;
	
		static TexturePtr loadTexture(const std::string& fileName);
		static void drawTexture(TexturePtr, const SDL_Rect * src, const SDL_FRect * dst, 
								double angle = 0, SDL_FPoint * center = nullptr, 
								SDL_RendererFlip flip = SDL_FLIP_NONE);
		static void unload(const std::string& path);
		static void clear();
};
