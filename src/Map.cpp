#include "Map.hpp"

#include "Game.hpp"
#include <fstream>
#include <string>

Map::Map() {
}

Map::~Map() {
}

void Map::loadMap(std::string path, int sizeX, int sizeY) {
	char tile;
	std::fstream mapFile;
	mapFile.open(path);
	
	for(int y = 0; y < sizeY; y++) {
		for (int x = 0; x < sizeX; x++) {
			mapFile.get(tile);
			Game::addTile(atoi(&tile),x*TILE_SIZE,y*TILE_SIZE);
			mapFile.ignore();
		}
	}
	
	mapFile.close();
}
