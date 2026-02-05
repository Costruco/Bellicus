#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <iostream>

#include "Game.hpp"
#include "FrameManager.hpp"
#include "EntityComponentSystem/Components.hpp"
#include "Map.hpp"
#include "TextureManager.hpp"
#include "Vector2D.hpp"

Game * game = nullptr;

int main(int argc, char * argv[]) {
	//inicializa o jogo
	game = new Game();
	game->init("Bellicus",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,640,true);
	
	//inicializa o frameManager
	FrameManager::init(60);
	
	while (game->running()) {
		if (FrameManager::frameDue()) {
			game->handleEvents();
			game->update();
			game->render();
		}
		
	}
	
	game->clean();
	return 0;
}
