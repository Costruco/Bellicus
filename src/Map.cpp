#include "Map.hpp"

#include "TextureManager.hpp"

float dragTable[] = {0.25f,0.17f};

int testLvl[MAX_MAP_X][MAX_MAP_y] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
					   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

Map::Map() {
	tile1 = TextureManager::loadTexture("assets/textures/tiles/tile_map_1.png");
	tile2 = TextureManager::loadTexture("assets/textures/tiles/tile_map_2.png");
	
	loadMap(testLvl);
	
	src = {0,0,100,100};
	dst = {0,0,100,100};
}

Map::~Map() {

}

void Map::loadMap(int arr[MAX_MAP_X][MAX_MAP_y]) {
	for (int i = 0; i < MAX_MAP_X; i++) {
		for (int j = 0; j < MAX_MAP_y; j++) {
			map[i][j] = arr[i][j];
		}
	}
}

int Map::getTile(int x, int y) {
	return this->map[x][y];
}

float getTileDrag(int x, int y) {
	return dragTable[x][y];
}

void Map::draw() {	 
	for (int i = 0; i < MAX_MAP_X; i++) {
		dst.x = i*100;
		for (int j = 0; j < MAX_MAP_y; j++) {
			dst.y = j*100;
			switch (map[i][j]) {
				case 0:
					TextureManager::drawTexture(tile1,&src,&dst,0,nullptr,SDL_FLIP_NONE);
					break;
				case 1:
					TextureManager::drawTexture(tile2,&src,&dst,0,nullptr,SDL_FLIP_NONE);
					break;
				default:
					break;
			}
		}
	}
}
