#include "Game.hpp"

Game * game = nullptr;

int main(int argc, char * argv[]) {
	game = new Game();
	
	game->init("Bellicus",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,600,60,false);
	
	int updateCounter = 0;
	while (game->running()) {
		
		if (SDL_GetTicks()%1000 == 0) {
			std::cout << updateCounter << std::endl;
			updateCounter = 0;
		}
		
		if (game->frameDue()) {
			updateCounter++;
			game->handleEvents();
			game->update();
			game->render();
		}
	}
	
	game->clean();
	return 0;
}
