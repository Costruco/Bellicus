#pragma once

#include "SDL2/SDL.h"
#include <vector>
#include "Camera.hpp"

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
		static Camera camera;

		int WINDOW_WIDTH;
		int WINDOW_HEIGHT;
		
		Game();
		~Game();
		
		void init(const char * title, int xpos, int ypos, int width, int height, bool fullscreen);
		
		void handleEvents();
		void update();
		void render();
		void clean();
		
		bool running();
		
		static void addTile(int srcx, int srcy, int xpos, int ypos);
};
