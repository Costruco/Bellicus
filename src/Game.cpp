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

std::vector<Entity*> wheels;
	
auto& newPlayer(manager.addEntity());

auto& wall(manager.addEntity());

enum GroupLabels : std::size_t {
	groupMap,
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
	wheels.emplace_back(&manager.addEntity());
	wheels.emplace_back(&manager.addEntity());
	wheels.emplace_back(&manager.addEntity());
	wheels.emplace_back(&manager.addEntity());
	Polygon car = Polygon{{-55,-30},{55,-30},{55,30},{-55,30}};
	wheels[0]->addGroup(groupPlayers);
	wheels[0]->addComponent<TransformComponent>(-55,-30,28,12);
	wheels[0]->addComponent<ColliderComponent>("wheel",Polygon{{-14,-6},{14,-6},{14,6},{-14,6}});
	wheels[0]->addComponent<SpriteComponent>("../assets/textures/entities/pneu.png");
	wheels[1]->addGroup(groupPlayers);
	wheels[1]->addComponent<TransformComponent>(55,-30,28,12);
	wheels[1]->addComponent<ColliderComponent>("wheel",Polygon{{-14,-6},{14,-6},{14,6},{-14,6}});
	wheels[1]->addComponent<SpriteComponent>("../assets/textures/entities/pneu.png");
	wheels[2]->addGroup(groupPlayers);
	wheels[2]->addComponent<TransformComponent>(55,30,28,12);
	wheels[2]->addComponent<ColliderComponent>("wheel",Polygon{{-14,-6},{14,-6},{14,6},{-14,6}});
	wheels[2]->addComponent<SpriteComponent>("../assets/textures/entities/pneu.png");
	wheels[3]->addGroup(groupPlayers);
	wheels[3]->addComponent<TransformComponent>(-55,30,28,12);
	wheels[3]->addComponent<ColliderComponent>("wheel",Polygon{{-14,-6},{14,-6},{14,6},{-14,6}});
	wheels[3]->addComponent<SpriteComponent>("../assets/textures/entities/pneu.png");
	
	
	newPlayer.addComponent<TransformComponent>(100,100,0.3f,180,88,1);
	newPlayer.addComponent<KeyboardController>();
	//newPlayer.addComponent<TankMovementComponent>(90.0f,54.0f,370000,36250,94000,0.095f,6,std::vector<Vector2D>{{-65,0},{0,0},{0,65},{65,135},{135,225},{225,487}});
	newPlayer.addComponent<CarMovementComponent>(150,120,110);
	//newPlayer.addComponent<SimpleMovementComponent>(100,45);
	
	newPlayer.addComponent<SpriteComponent>("../assets/textures/entities/carro.png",2,500);
	//newPlayer.addComponent<SpriteComponent>("../assets/textures/entities/T-34/chassi_com_sombra.png");
	newPlayer.addComponent<ColliderComponent>("player",Polygon{{-79,-32},{79,-32},{79,32},{-79,32}});
	newPlayer.addGroup(groupPlayers);
	
	wall.addComponent<TransformComponent>(200,200,0.0f,200,300,1);
	wall.addComponent<SpriteComponent>("../assets/textures/entities/brick_wall.png");
	wall.addComponent<ColliderComponent>("wall",Polygon{{-100,-150},{100,-150},{100,150},{-100,150}});
	wall.addGroup(groupMap);
	
	wheels[0]->getComponent<TransformComponent>().setFather(&newPlayer.getComponent<TransformComponent>());
	wheels[1]->getComponent<TransformComponent>().setFather(&newPlayer.getComponent<TransformComponent>());
	wheels[2]->getComponent<TransformComponent>().setFather(&newPlayer.getComponent<TransformComponent>());
	wheels[3]->getComponent<TransformComponent>().setFather(&newPlayer.getComponent<TransformComponent>());
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
	

	wheels[1]->getComponent<TransformComponent>().direction = newPlayer.getComponent<CarMovementComponent>().wheelDirection;
	wheels[2]->getComponent<TransformComponent>().direction = newPlayer.getComponent<CarMovementComponent>().wheelDirection;
	
	
	//std::cout << newPlayer.getComponent<TransformComponent>().position.x << std::endl;
	for (auto c : colliders) {
		if (Collision::SAT(newPlayer.getComponent<ColliderComponent>(),*c)) {
			//std::cout << "Bateu!" << std::endl;
		}
	}
}

auto& tiles(manager.getGroup(groupMap));
auto& players(manager.getGroup(groupPlayers));
auto& enemies(manager.getGroup(groupEnemies));

void Game::render() {
	SDL_SetRenderDrawColor(ren,255,255,255,255);
	SDL_RenderClear(ren);
	for (auto& t : tiles) {
		t->draw();
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
