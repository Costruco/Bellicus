#include "Game.hpp"

SDL_Texture * playerTex;
SDL_Rect srcR,destR;

Game::Game() {
	updateCounter = 0;
}

Game::~Game() {

}

void Game::init(const char * title, int xpos, int ypos, int width, int height, int targetFPS, bool fullscreen) {
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
				SDL_SetRenderDrawColor(ren,0,0,0,255);
				std::cout << "Renderer created...\n" << "Game started..." << std::endl;
				
				isRunning = true;
			}
		}
	} else
		isRunning = false;
		
	//inicializa o frameManager
	FrameManager::init(targetFPS);
	
	//inicializa texturas
	playerTex = TextureManager::LoadTexture(ren,"assets/textures/entities/T-34/chassi_com_sombra.png");
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
	destR.w = 203;
	destR.h = 104;
}

void Game::render() {
	SDL_RenderClear(ren);
	SDL_RenderCopy(ren,playerTex,NULL,&destR);
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
