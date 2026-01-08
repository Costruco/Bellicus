#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "Game.hpp"

class GameObject {
	public:
		GameObject(SDL_Renderer * ren, const char * fileName, int x, int y);
		~GameObject();
		
		void update();
		void render();
		
	private:
		SDL_Point local;
		
		SDL_Renderer * ren;
		
		SDL_Texture * texture;
		SDL_Rect srcRect;
		SDL_Rect dstRect;
};

#endif
