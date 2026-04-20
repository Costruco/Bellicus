#pragma once

#include <string>
#define TILE_SIZE 100

class Map {		
	public:
		Map();
		~Map();
		
		static void loadMap(std::string path, int sizeX, int sizeY);
};
