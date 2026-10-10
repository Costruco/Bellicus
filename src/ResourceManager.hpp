#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <unordered_map>
#include <string>
#include <memory>

class ResourceManager {
	private:
		static std::unordered_map<std::string,std::weak_ptr<SDL_Texture>> textureCache;
		static std::unordered_map<std::string,std::weak_ptr<TTF_Font>> fontCache;
		static std::unordered_map<std::string,std::weak_ptr<Mix_Chunk>> soundCache;
	public:
		using TexturePtr = std::shared_ptr<SDL_Texture>;
		using FontPtr = std::shared_ptr<TTF_Font>;
		using SoundPtr = std::shared_ptr<Mix_Chunk>;
	
		static TexturePtr loadTexture(const std::string& fileName);
		static FontPtr loadFont(const std::string& fileName, int fontSize);
		static SoundPtr loadSound(const std::string& fileName);
		static void drawTexture(TexturePtr, const SDL_Rect * src, const SDL_FRect * dst, 
								double angle = 0, SDL_FPoint * center = nullptr, 
								SDL_RendererFlip flip = SDL_FLIP_NONE);
		static void unloadTexture(const std::string& path);
		static void unloadFont(const std::string& path);
		static void unloadSound(const std::string& path);
		static void clear();
};
