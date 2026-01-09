#include "Game.hpp"

Map * map;

GameObject * player;

SDL_Renderer * Game::ren = nullptr;

Game::Game() {
	updateCounter = 0;
}

Game::~Game() {

}

void Game::init(const char * title, int xpos, int ypos, int width, int height, bool fullscreen) {
	//inicializa as flags
	int flags = 0;
	if (fullscreen)
		flags = flags | SDL_WINDOW_FULLSCREEN;
	//inicializa o sdl
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Subsystem initialized..." << std::endl;
		//cria janela
		win = SDL_CreateWindow(title,xpos,ypos,width,height,flags);
		if (win) {
			std::cout << "Window created..." << std::endl;
			//cria renderizador
			ren = SDL_CreateRenderer(win,-1,0);
			if (ren) {
				SDL_SetRenderDrawColor(ren,255,255,255,255);
				std::cout << "Renderer created...\n" << "Game started..." << std::endl;
				
				isRunning = true;
			}
		}
	} else
		isRunning = false;
	
	//inicializa texturas
	player = new GameObject("assets/textures/entities/T-34/chassi_com_sombra.png",100,100);
	map = new Map();
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
	player->update();
}

void Game::render() {
	SDL_RenderClear(ren);
	map->render();
	player->render();
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
