#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

#include "FrameManager.hpp"
#include "Game.hpp"

Game * game = nullptr;

int main(int argc, char * argv[]) {
	//inicializa o jogo
	game = new Game();
	game->init("Bellicus",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1600,900,true);
	
	//inicializa o frameManager (0 = sem limite de FPS)
	FrameManager::init(0);
	
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