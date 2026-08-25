#include "Game.hpp"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <iostream>
#include <vector>

#include "FrameManager.hpp"
#include "TextureManager.hpp"
#include "Map.hpp"
#include "Camera.hpp"

#include "Vector2D.hpp"
#include "EntityComponentSystem/Components.hpp"

#include "Collision.hpp"

Manager manager;

SDL_Renderer * Game::ren = nullptr;
SDL_Event Game::evt;
std::vector<ColliderComponent*> Game::colliders;
const Uint8* Game::keystate = nullptr;
Camera Game::camera;
	
auto& newPlayer(manager.addEntity());
auto& wall(manager.addEntity());

const std::string mapfile = "../assets/textures/tiles/tile_map_1.png";

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
		SDL_GetWindowSize(win,&WINDOW_WIDTH,&WINDOW_HEIGHT);
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
	/*newPlayer.addComponent<TransformComponent>(100,100,0.0f,180,88,1);
	newPlayer.addComponent<SpriteComponent>("../assets/textures/entities/carro.png",2,500);
	newPlayer.addComponent<ColliderComponent>("player",Polygon{{-79,-32},{79,-32},{79,32},{-79,32}});*/
	
	newPlayer.addComponent<TransformComponent>(100,100,0.0f,/*220,118*/180,88,1);
	newPlayer.addComponent<SpriteComponent>("../assets/textures/entities/carro.png",true);
	newPlayer.addComponent<ColliderComponent>("player",Polygon{{-79,-32},{79,-32},{79,32},{-79,32}});
	
	newPlayer.addComponent<KeyboardController>();
	
	CarMovementConfig carConfig(260.0f,1102.0f,74.6f,64.75f,64.75f,27.5f,15.8f,0.92f,900.0f,800.0f,6500.0f,
								5.0f,3.0f,0.7f,12.0f,18.0f,120.0f,0.45f,
								38.0f,0.055f,1.15f,24.0f,45.0f,45.0f,22.0f,22.0f,
								Vector2D(),Vector2D(),Vector2D(),0.0f,0.0f,
								TorqueCurve(157.0f,70.0f,4000.0f,1.0f,6000.0f),
								GearBox({-3.545f,0.0f,3.727f,2.048f,1.393f,1.029f,0.795f},4.067f,2),
								PacejkaCurve(13.5f,1.9f,1.0f,0.92f),
								PacejkaCurve(7.0f,1.55f,0.88f,0.78f),
								{CarWheelConfig(Vector2D(64.75f,-37.3f),Vector2D(28.0f,12.0f),true,true),
								 CarWheelConfig(Vector2D(64.75f,37.3f),Vector2D(28.0f,12.0f),true,true),
								 CarWheelConfig(Vector2D(-64.75f,-37.3f),Vector2D(28.0f,12.0f),false,false),
								 CarWheelConfig(Vector2D(-64.75f,37.3f),Vector2D(28.0f,12.0f),false,false)});
	
	newPlayer.addComponent<CarMovementComponent>(&manager,carConfig,"../assets/textures/entities/pneu.png",groupGround);
	newPlayer.addGroup(groupPlayers);
	
	wall.addComponent<TransformComponent>(200,200,45.0f,200,300,1);
	wall.addComponent<SpriteComponent>("../assets/textures/entities/brick_wall.png");
	wall.addComponent<ColliderComponent>("wall",Polygon{{-100,-150},{100,-150},{100,150},{-100,150}});
	wall.addGroup(groupMap);
}

void Game::handleEvents() {
	static int mouseX = 0, mouseY = 0;

	float beforeWorldMouseX, beforeWorldMouseY;
	camera.screenToWorld((float)mouseX,(float)mouseY,beforeWorldMouseX,beforeWorldMouseY);

	SDL_PollEvent(&evt);
	SDL_GetMouseState(&mouseX,&mouseY);
	
	float afterWorldMouseX, afterWorldMouseY;
	camera.screenToWorld((float)mouseX,(float)mouseY,afterWorldMouseX,afterWorldMouseY);
	
	switch (evt.type) {
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			if (evt.key.keysym.sym == SDLK_ESCAPE)
				camera.xMouseOffset = camera.yMouseOffset = 0.0f;
			break;
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_MOUSEWHEEL:
			if (evt.wheel.y > 0 && camera.xScale+0.1f <= 5.0f) {
				camera.xScale += 0.1f;
				camera.yScale += 0.1f;
			} else if (evt.wheel.y < 0 && camera.xScale-0.1f >= 0.1f) {
				camera.xScale -= 0.1f;
				camera.yScale -= 0.1f;
			}
			
			camera.xMouseOffset += afterWorldMouseX-beforeWorldMouseX;
			camera.yMouseOffset += afterWorldMouseY-beforeWorldMouseY;
			break;
		default:
			break;
	}
	std::cout << "Camera scale: " << camera.xScale << std::endl;
	std::cout << "Mouse screen position: " << mouseX << "," << mouseY << std::endl;
	std::cout << "Before world position: " << beforeWorldMouseX << "," << beforeWorldMouseY <<std::endl;
	std::cout << "After world position: " << afterWorldMouseX << "," << afterWorldMouseY << std::endl;
	std::cout << "Offset: " << camera.xMouseOffset+camera.xOffset << "," << camera.yMouseOffset+camera.yOffset << std::endl;
	keystate = SDL_GetKeyboardState(NULL);
}

void Game::update() {
	manager.refresh();
	manager.update();

	camera.xOffset = WINDOW_WIDTH/2;
	camera.yOffset = WINDOW_HEIGHT/2;
	SDL_FPoint playerPos = newPlayer.getComponent<TransformComponent>().getPosition();
	camera.xZoomCenter = playerPos.x;
	camera.yZoomCenter = playerPos.y;
	if (newPlayer.getComponent<CarMovementComponent>().velocity.getModule() > 1)
		newPlayer.getComponent<SpriteComponent>().play("walk");
	else
		newPlayer.getComponent<SpriteComponent>().play("idle");
	
	updateCounter++;

	if (!(updateCounter%60)) {
		//std::cout << newPlayer.getComponent<TransformComponent>().position.x << std::endl;
		//std::cout << "---------------------------------------------------------------------------------------" << std::endl;
		//std::cout << newPlayer.getComponent<CarMovementComponent>().velocity.getModule()/PIXELS_PER_METER*3.6f << "Km/h" << std::endl;
		//std::cout << newPlayer.getComponent<CarMovementComponent>().gearbox.gear << std::endl;
		//std::cout << newPlayer.getComponent<CarMovementComponent>().engineRPM << std::endl;
	}
	
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

void Game::addTile(int srcx, int srcy, int xpos, int ypos) {
	auto& tile(manager.addEntity());
	tile.addComponent<TileComponent>(srcx,srcy,xpos,ypos,mapfile);
	tile.addGroup(groupMap);
}
