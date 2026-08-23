#include "Map.hpp"

#include "Game.hpp"
#include <fstream>
#include <string>

Map::Map() {
}

Map::~Map() {
}

void Map::loadMap(const std::string& path, int sizeX, int sizeY) {
	char tile;
	std::fstream mapFile;
	mapFile.open(path);
	
	int srcx,srcy;
	for(int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			mapFile.get(tile);
			srcx = atoi(&tile)*TILE_SIZE;
			srcy = atoi(&tile)*TILE_SIZE;
			Game::addTile(srcx,srcy,x*TILE_SIZE,y*TILE_SIZE);
			mapFile.ignore();
		}
	}
	
	mapFile.close();
}
