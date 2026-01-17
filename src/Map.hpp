#ifndef MAP_HPP
#define MAP_HPP

#include "Game.hpp"

class Map {
	private:
		SDL_Rect src,
				 dst;
		
		SDL_Texture * tile1,
					* tile2;
		
		int map[20][20];
		
	public:
		Map();
		~Map();
		
		void loadMap(int arr[20][20]);
		void draw();	
};

#endif
