#ifndef GAME_HPP
#define GAME_HPP

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <iostream>

#include "FrameManager.hpp"
#include "TextureManager.hpp"

class Game {
	public:
		Game();
		~Game();
		
		void init(const char * title, int xpos, int ypos, int width, int height, int targetFPS, bool fullscreen);
		
		void handleEvents();
		void update();
		void render();
		void clean();
		
		bool running();
		
	private:
		int updateCounter;
		bool isRunning;
		SDL_Window * win;
		SDL_Renderer * ren;
};

#endif
