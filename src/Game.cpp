#include "Game.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <iostream>
#include <vector>

#include "FrameManager.hpp"
#include "TextureManager.hpp"
#include "Map.hpp"

#include "Vector2D.hpp"
#include "EntityComponentSystem/Components.hpp"

#include "Collision.hpp"

Manager manager;

SDL_Renderer * Game::ren = nullptr;
SDL_Event Game::evt;
std::vector<ColliderComponent*> Game::colliders;
const Uint8* Game::keystate = nullptr;
	
auto& newPlayer(manager.addEntity());
auto& wall(manager.addEntity());

enum GroupLabels : std::size_t {
	groupMap,
	groupGround,
	groupPlayers,
	groupEnemies,
	groupColliders
};

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
				SDL_SetRenderDrawBlendMode(ren,SDL_BLENDMODE_BLEND);
				SDL_SetRenderDrawColor(ren,255,255,255,255);
				std::cout << "Renderer created...\n" << "Game started..." << std::endl;
				
				isRunning = true;
			}
		}
	} else
		isRunning = false;
	
	Map::loadMap("../assets/maps/map1.txt",10,10);
	
	//inicializa entidades
	newPlayer.addComponent<TransformComponent>(100,100,0.0f,180,88,1);
	newPlayer.addComponent<KeyboardController>();
	
	CarMovementConfig carConfig(260.0f,1280.0f,75.0f,58.0f,58.0f,30.0f,17.5f,0.92f,1200.0f,900.0f,7200.0f,
								10.0f,3.0f,1.8f,12.0f,18.0f,120.0f,0.45f,
								42.0f,0.5f,0.001f,1.35f,24.0f,45.0f,45.0f,22.0f,
								Vector2D(),Vector2D(),Vector2D(),0.0f,0.0f,
								TorqueCurve(430.0f,95.0f,3800.0f,1.0f,2800.0f),
								GearBox({-3.10f,0.0f,3.23f,2.24f,1.59f,1.21f,1.00f,0.82f},4.35f,2),
								PacejkaCurve(13.5f,1.9f,1.0f,0.92f),
								PacejkaCurve(7.0f,1.55f,0.88f,0.78f),
								{CarWheelConfig(Vector2D(55,-25),Vector2D(28,12),true,true),
								 CarWheelConfig(Vector2D(55,25),Vector2D(28,12),true,true),
								 CarWheelConfig(Vector2D(-55,-25),Vector2D(28,12),false,true),
								 CarWheelConfig(Vector2D(-55,25),Vector2D(28,12),false,true)});
	
	newPlayer.addComponent<CarMovementComponent>(&manager,carConfig,"../assets/textures/entities/pneu.png",groupGround);
	newPlayer.addComponent<SpriteComponent>("../assets/textures/entities/carro.png",2,500);
	newPlayer.addComponent<ColliderComponent>("player",Polygon{{-79,-32},{79,-32},{79,32},{-79,32}});
	newPlayer.addGroup(groupPlayers);
	
	wall.addComponent<TransformComponent>(200,200,0.0f,200,300,1);
	wall.addComponent<SpriteComponent>("../assets/textures/entities/brick_wall.png");
	wall.addComponent<ColliderComponent>("wall",Polygon{{-100,-150},{100,-150},{100,150},{-100,150}});
	wall.addGroup(groupMap);
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
	keystate = SDL_GetKeyboardState(NULL);
}

void Game::update() {
	manager.refresh();
	manager.update();

	//std::cout << newPlayer.getComponent<TransformComponent>().position.x << std::endl;
	std::cout << newPlayer.getComponent<CarMovementComponent>().velocity.getModule()/PIXELS_PER_METER*3.6f << "Km/h" << std::endl;
	std::cout << newPlayer.getComponent<CarMovementComponent>().gearbox.gear << std::endl;
	std::cout << newPlayer.getComponent<CarMovementComponent>().engineRPM << std::endl;
	for (auto c : colliders) {
		if (Collision::SAT(newPlayer.getComponent<ColliderComponent>(),*c)) {
			//std::cout << "Bateu!" << std::endl;
		}
	}
}

auto& tiles(manager.getGroup(groupMap));
auto& ground(manager.getGroup(groupGround));
auto& players(manager.getGroup(groupPlayers));
auto& enemies(manager.getGroup(groupEnemies));

void Game::render() {
	SDL_SetRenderDrawColor(ren,255,255,255,255);
	SDL_RenderClear(ren);
	for (auto& t : tiles) {
		t->draw();
	}
	for (auto& g : ground) {
		g->draw();
	}
	for (auto& p : players) {
		p->draw();
	}
	for (auto& e : enemies) {
		e->draw();
	}
	SDL_RenderPresent(ren);
}

void Game::clean() {
	TextureManager::clear();
	manager.clear();
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
	std::cout << "Game cleaned." << std::endl;
}

bool Game::running() {
	return isRunning;
}

void Game::addTile(int id, int x, int y) {
	auto& tile(manager.addEntity());
	tile.addComponent<TileComponent>(x,y,TILE_SIZE,TILE_SIZE,id);
	tile.addGroup(groupMap);
}
