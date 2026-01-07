#include "Game.hpp"

Game * game = nullptr;

int main(int argc, char * argv[]) {
	game = new Game();
	
	game->init("Bellicus",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,600,60,false);
	
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
