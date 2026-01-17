#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "Game.hpp"

class GameObject {
	private:
		int xpos,
			ypos;
		
		SDL_Texture * texture;
		SDL_Rect src,
				 dst;
				 
	public:
		GameObject(const char * fileName, int x, int y);
		~GameObject();
		
		void update();
		void draw();
};

#endif
