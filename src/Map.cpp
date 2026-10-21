#include <fstream>
#include <string>

#include "Game.hpp"
#include "Map.hpp"
#include "Utils/String.hpp"

Map::Map() {
}

Map::~Map() {
}

void Map::loadMap(const std::string& path, int &mapSizeX, int &mapSizeY) {
	char tile;
	std::fstream mapFile;
	mapFile.open(path);
	
	int srcx,srcy;
	getNumber(mapFile,mapSizeX);
	getNumber(mapFile,mapSizeY);
	mapFile.ignore(2,'\n');
	for(int y = -mapSizeX/2; y < mapSizeY-mapSizeY/2; y++) {
		for (int x = -mapSizeX/2; x < mapSizeX-mapSizeY/2; x++) {
			mapFile.get(tile);
			srcx = (tile-'0')*TILE_SIZE;
			mapFile.get(tile);
			srcy = (tile-'0')*TILE_SIZE;
			Game::addTile(srcx,srcy,x*TILE_SIZE,y*TILE_SIZE);
			mapFile.ignore();
		}
		mapFile.ignore(2,'\n');
	}
	
	mapFile.close();
}