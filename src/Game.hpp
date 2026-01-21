#ifndef GAME_HPP
#define GAME_HPP

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <iostream>

#include "EntityComponentSystem/Components.hpp"
#include "FrameManager.hpp"
#include "Map.hpp"
#include "TextureManager.hpp"
#include "Vector2D.hpp"


class Game {
	private:
		int updateCounter;
		bool isRunning;
		SDL_Window * win;
		
	public:
		static SDL_Renderer * ren;
		
		Game();
		~Game();
		
		void init(const char * title, int xpos, int ypos, int width, int height, bool fullscreen);
		
		void handleEvents();
		void update();
		void render();
		void clean();
		
		bool running();
};

#endif
