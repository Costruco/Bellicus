#ifndef MAP_HPP
#define MAP_HPP

#include "SDL2/SDL.h"

#define MAX_MAP_X 20
#define MAX_MAP_Y 20

class Map {
	private:
		SDL_Rect src,
				 dst;
		
		SDL_Texture * tile1,
					* tile2;
		
		int map[MAX_MAP_X][MAX_MAP_Y];
		
	public:
		Map();
		~Map();
		
		void loadMap(int arr[MAX_MAP_X][MAX_MAP_Y]);
		int getTile(int x, int y);
		float getTileDrag(int x, int y);
		void draw();	
};

#endif
