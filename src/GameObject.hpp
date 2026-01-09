#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "Game.hpp"

class GameObject {
	public:
		GameObject(const char * fileName, int x, int y);
		~GameObject();
		
		void update();
		void render();
		
	private:
		SDL_Point local;
		
		SDL_Texture * texture;
		SDL_Rect src,
				 dst;
};

#endif
