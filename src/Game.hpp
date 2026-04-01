#pragma once

#include "SDL2/SDL.h"

class Game {
	private:
		int updateCounter;
		bool isRunning;
		SDL_Window * win;
		
	public:
		static SDL_Renderer * ren;
		static SDL_Event evt;
		
		Game();
		~Game();
		
		void init(const char * title, int xpos, int ypos, int width, int height, bool fullscreen);
		
		void handleEvents();
		void update();
		void render();
		void clean();
		
		bool running();
};
