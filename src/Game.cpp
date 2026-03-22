#include "Game.hpp"

#include "SDL2/SDL_image.h"

#include <iostream>

#include "FrameManager.hpp"
#include "TextureManager.hpp"
#include "Map.hpp"

#include "Vector2D.hpp"
#include "EntityComponentSystem/Components.hpp"

Map * map;
Manager manager;

SDL_Renderer * Game::ren = nullptr;
SDL_Event Game::evt;

auto& newPlayer(manager.addEntity());
auto& wheel(manager.addEntity());

Game::Game() {
	updateCounter = 0;
}

Game::~Game() {

}

void Game::init(const char * title, int xpos, int ypos, int width, int height, bool fullscreen) {
	//inicializa as flags
	int flags = SDL_WINDOW_SHOWN;
	if (fullscreen)
		flags = flags | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP;

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
	map = new Map();
	
	newPlayer.addComponent<TransformComponent>(100,100,0.3f);
	newPlayer.addComponent<KeyboardController>();
	//newPlayer.addComponent<TankMovementComponent>(0.0f,54.0f,370000,36250,94000,0.095f,*map,6,std::vector<SDL_Point>{{-65,0},{0,0},{0,65},{65,135},{135,225},{225,487}});
	//newPlayer.addComponent<SpriteComponent>("assets/textures/entities/T-34/chassi_com_sombra.png");
	newPlayer.addComponent<CarMovementComponent>(100,30,2);
	newPlayer.addComponent<SpriteComponent>("assets/textures/entities/carro.png");
	
	wheel.addComponent<TransformComponent>(100,100,0.3f);
}

void Game::handleEvents() {
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
	manager.refresh();
	//std::cout << newPlayer.getComponent<TransformComponent>().position << std::endl;
	//std::cout << newPlayer.getComponent<TransformComponent>().direction << std::endl;
	manager.update();
}

void Game::render() {
	SDL_RenderClear(ren);
	map->draw();
	manager.draw();
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
