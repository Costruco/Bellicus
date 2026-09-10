#include "Map.hpp"

#include "Game.hpp"
#include <fstream>
#include <string>

Map::Map() {
}

Map::~Map() {
}

void Map::loadMap(const std::string& path, int &mapSizeX, int &mapSizeY) {
	char tile;
	std::fstream mapFile;
	mapFile.open(path);
	
	int sizeX,sizeY,srcx,srcy;
	mapFile >> sizeX >> sizeY;
	mapSizeX = sizeX;
	mapSizeY = sizeY;
	for(int y = -sizeY/2; y < sizeY-sizeY/2; y++) {
		for (int x = -sizeX/2; x < sizeX-sizeX/2; x++) {
			mapFile.get(tile);
			srcx = atoi(&tile)*TILE_SIZE;
			mapFile.get(tile);
			srcy = atoi(&tile)*TILE_SIZE;
			Game::addTile(srcx,srcy,x*TILE_SIZE,y*TILE_SIZE);
			mapFile.ignore();
		}
		mapFile.ignore(2,'\n');
	}
	
	mapFile.close();
}
