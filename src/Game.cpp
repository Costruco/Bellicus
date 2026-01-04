#include "Game.hpp"

Game::Game() {
	updateCounter = 0;
}

Game::~Game() {

}

void Game::init(const char * title, int xpos, int ypos, int width, int height, bool fullscreen) {
	int flags = 0;
	if (fullscreen)
		flags = flags | SDL_WINDOW_FULLSCREEN;
	
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Subsystem initialized..." << std::endl;
		
		win = SDL_CreateWindow(title,xpos,ypos,width,height,flags);
		if (win) {
			std::cout << "Window created..." << std::endl;
		
			ren = SDL_CreateRenderer(win,-1,0);
			if (ren) {
				SDL_SetRenderDrawColor(ren,0,0,0,255);
				std::cout << "Renderer created..." << std::endl;
				
				isRunning = true;
			}
		}
	} else
		isRunning = false;
}

void Game::handleEvents() {
	SDL_Event evt;
	SDL_PollEvent(&evt);
	switch (evt.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		default:
			break;
	}
}

void Game::update() {
	updateCounter++;
	std::cout << updateCounter << std::endl;
}

void Game::render() {
	SDL_RenderClear(ren);
	SDL_RenderPresent(ren);
}

void Game::clean() {
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	std::cout << "Game cleaned." << std::endl;
}

bool Game::running() {
	return isRunning;
}
		
