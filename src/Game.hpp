#pragma once

#include "SDL2/SDL.h"
#include <vector>

class ColliderComponent;

class Game {
	private:
		int updateCounter;
		bool isRunning;
		SDL_Window * win;
		
	public:
		static SDL_Renderer * ren;
		static SDL_Event evt;
		static std::vector<ColliderComponent*> colliders;
		static const Uint8* keystate;
		
		Game();
		~Game();
		
		void init(const char * title, int xpos, int ypos, int width, int height, bool fullscreen);
		
		void handleEvents();
		void update();
		void render();
		void clean();
		
		bool running();
};
