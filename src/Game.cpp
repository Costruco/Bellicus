#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <vector>

#include "Camera.hpp"
#include "Collision.hpp"
#include "EntityComponentSystem/Components.hpp"
#include "FrameManager.hpp"
#include "Game.hpp"
#include "Map.hpp"
#include "ResourceManager.hpp"
#include "Vector2D.hpp"

Manager manager;

SDL_Renderer * Game::ren = nullptr;
SDL_Texture * Game::canva = nullptr;
int Game::WINDOW_WIDTH = 0;
int Game::WINDOW_HEIGHT = 0;
SDL_Event Game::evt;
std::vector<ColliderComponent*> Game::colliders;
const Uint8* Game::keystate = nullptr;
Camera Game::camera;
	
auto& newPlayer(manager.addEntity());
auto& wall(manager.addEntity());
auto& vaga(manager.addEntity());
auto& info(manager.addEntity());

int mapSizeX,mapSizeY;
const std::string mapfile = "../assets/textures/tiles/tile_map_1.png";

enum GroupLabels : std::size_t {
	groupMap,
	groupGround,
	groupPlayers,
	groupEnemies,
	groupColliders,
	groupUI
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
		flags = flags | SDL_WINDOW_FULLSCREEN_DESKTOP;

	//inicializa o sdl
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0 && 
		IMG_Init(IMG_INIT_PNG) == IMG_INIT_PNG && 
		TTF_Init() == 0 && 
		Mix_Init(MIX_INIT_MP3) == MIX_INIT_MP3) {
		std::cout << "Subsystem initialized..." << std::endl;
		//cria janela
		win = SDL_CreateWindow(title,xpos,ypos,width,height,flags);
		
		
		if (win) {
			SDL_GetWindowSize(win,&WINDOW_WIDTH,&WINDOW_HEIGHT);
			std::cout << "Window created with screen logical size "<< width<<"x"<<height << "..." << std::endl;
			std::cout << "Window created with screen real size "<< WINDOW_WIDTH<<"x"<<WINDOW_HEIGHT << "..." << std::endl;
			logicalScale = {static_cast<float>(width)/WINDOW_WIDTH, static_cast<float>(height)/WINDOW_HEIGHT};
			WINDOW_WIDTH = width; WINDOW_HEIGHT = height;
			//cria renderizador
			ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
			canva = SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,WINDOW_WIDTH,WINDOW_HEIGHT);
			if (ren) {
				SDL_SetRenderDrawBlendMode(ren,SDL_BLENDMODE_BLEND);
				SDL_SetRenderDrawColor(ren,0,0,0,255);
				std::cout << "Renderer created...\n" << "Game started..." << std::endl;
				
				isRunning = true;
			}
		}
	} else
		isRunning = false;
	
	Map::loadMap("../assets/maps/map1.map",mapSizeX,mapSizeY);
	
	//inicializa entidades
	/*newPlayer.addComponent<TransformComponent>(100,100,0.0f,180,88,1);
	newPlayer.addComponent<SpriteComponent>("../assets/textures/entities/carro.png",2,500);
	newPlayer.addComponent<ColliderComponent>("player",Polygon{{-79,-32},{79,-32},{79,32},{-79,32}});*/
	
	newPlayer.addComponent<TransformComponent>(0,0,0.0f,/*220,118*/180,88,1);
	newPlayer.addComponent<SpriteComponent>("../assets/textures/entities/carro.png",true);
	newPlayer.addComponent<ColliderComponent>("player",158,64);
	
	newPlayer.addComponent<KeyboardController>();
	
	newPlayer.addComponent<CarMovementComponent>(&manager,
												 260.0f,1102.0f,104.0f,48.0f,27.5f,15.8f,0.92f,800.0f,6500.0f,
												 5.0f,3.0f,0.7f,12.0f,18.0f,120.0f,0.10f,
												 38.0f,0.025f,1.15f,1.0f,45.0f,45.0f,22.0f,22.0f,
												 TorqueCurve(157.0f,70.0f,4000.0f,1.0f,6000.0f),
												 GearBox({-3.545f,0.0f,3.727f,2.048f,1.393f,1.029f,0.795f},4.067f),
												 PacejkaCurve(13.5f,1.9f,1.0f,0.92f),
												 PacejkaCurve(7.0f,1.55f,0.88f,0.78f),
												 std::vector<CarWheelConfig>{CarWheelConfig(Vector2D(31.6f,10.3f),true,false),
																			 CarWheelConfig(true,false),
																			 CarWheelConfig(false,true),
																			 CarWheelConfig(false,true)},
												 "../assets/textures/entities/pneu.png",groupGround);
	newPlayer.addGroup(groupPlayers);
	
	wall.addComponent<TransformComponent>(200,200,45.0f,200,300,1);
	wall.addComponent<SpriteComponent>("../assets/textures/entities/brick_wall.png");
	wall.addComponent<ColliderComponent>("wall");
	wall.addGroup(groupMap);

	vaga.addComponent<TransformComponent>(-200,-200,0.0f,637,90,1);
	vaga.addComponent<SpriteComponent>("../assets/textures/features/vaga.png");
	vaga.addGroup(groupGround);

	info.addComponent<MessageBoxComponent>("../assets/fonts/sdl2_gfx_8x8.ttf",8,
											std::vector<std::string>{
											"x: /right[:1.0f]",
											"y: /right[:1.0f]",
											"Angulo: /right[:1.2f]",
											"Velocidade: /right[:1.1f]",
											"Pontos: /right[:1.0f]"},
											ScreenAlignment::TOP_LEFT);
	info.getComponent<MessageBoxComponent>().setColor({255,255,255,255},{255,255,255,255},{0,0,0,255});
	info.addGroup(groupUI);
}

void Game::handleEvents() {
	static Vector2D mouse;
	int mouseX,mouseY;

	Vector2D beforeWorldMouse = camera.screenToWorld(mouse*logicalScale);
	SDL_GetMouseState(&mouseX,&mouseY);
	mouse = {mouseX,mouseY};
	Vector2D afterWorldMouse = camera.screenToWorld(mouse*logicalScale);
	while (SDL_PollEvent(&evt)) {
		switch (evt.type) {
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				if (evt.key.keysym.sym == SDLK_ESCAPE)
					camera.mouseOffset = Vector2D();
				break;
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_MOUSEWHEEL:
				if (evt.wheel.y > 0 && camera.scale.x+0.1f <= 2.0f)
					camera.scale += {0.1f,0.1f};
				else if (evt.wheel.y < 0 && camera.scale.x-0.1f >= 0.5f)
					camera.scale -= {0.1f,0.1f};
				
				camera.mouseOffset += {afterWorldMouse.x-beforeWorldMouse.x,
									   afterWorldMouse.y-beforeWorldMouse.y};
				break;
			default:
				break;
		}
	}
	//std::cout << "Camera scale: " << camera.xScale << std::endl;
	//std::cout << "Mouse screen position: " << mouseX << "," << mouseY << std::endl;
	//std::cout << "Before world position: " << beforeWorldMouseX << "," << beforeWorldMouseY <<std::endl;
	//std::cout << "After world position: " << afterWorldMouseX << "," << afterWorldMouseY << std::endl;
	//std::cout << "Offset: " << camera.xMouseOffset+camera.xOffset << "," << camera.yMouseOffset+camera.yOffset << std::endl;
	keystate = SDL_GetKeyboardState(NULL);
}

void Game::update() {
	manager.refresh();
	manager.update();

	CarMovementComponent& car = newPlayer.getComponent<CarMovementComponent>();
	float velocity = car.velocity.getModule()/PIXELS_PER_METER*3.6f;
	float rpm = car.engineRPM;
	float gear = car.gearbox.gear;
	info.getComponent<MessageBoxComponent>().setValues(std::vector<std::vector<float>>{
													   {0},
													   {-0},
													   {0},
													   {0},
													   {0}});
	
	camera.offset = {WINDOW_WIDTH/2,WINDOW_HEIGHT/2};
	camera.angle = 90+newPlayer.getComponent<TransformComponent>().getDirection();
	SDL_FPoint playerPos = newPlayer.getComponent<TransformComponent>().getPosition();

	//destroi o carro para testar fim de jogo e destruicao de entidades
	if (playerPos.x > 800) {
		newPlayer.destroy();
		manager.refresh();
		Game::render();
		SDL_Delay(1000);
		isRunning = false;
		return;
	}

	//bloqueia a visão da parte exterior do mapa
	float left = -mapSizeX/2*TILE_SIZE;
	float right = (mapSizeX-mapSizeX/2)*TILE_SIZE;
	float top = -mapSizeY/2*TILE_SIZE;
	float bottom = (mapSizeY-mapSizeY/2)*TILE_SIZE;

	float cos = std::abs(cosd(camera.angle));
	float sin = std::abs(sind(camera.angle));
	float viewW = WINDOW_WIDTH/camera.scale.x;
	float viewH = WINDOW_HEIGHT/camera.scale.y;
	float halfSizeX = (viewW*cos+viewH*sin)*0.5f;
    float halfSizeY = (viewW*sin+viewH*cos)*0.5f;

	left = left+halfSizeX;
	right = right-halfSizeX;
	top = top+halfSizeY;
	bottom = bottom-halfSizeY;

	camera.focus.x = (left>right)?(left+right)*0.5f:clamp(playerPos.x,left,right);
	camera.focus.y = (top>bottom)?(top+bottom)*0.5f:clamp(playerPos.y,top,bottom);

	if (newPlayer.getComponent<CarMovementComponent>().velocity.getModule() > 1)
		newPlayer.getComponent<SpriteComponent>().play("walk");
	else
		newPlayer.getComponent<SpriteComponent>().play("idle");
	
	updateCounter++;

	if (updateCounter == 10) {
		//std::cout << Vector2D(playerPos) << std::endl;
		//std::cout << newPlayer.getComponent<TransformComponent>().getDirection() << std::endl;
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
auto& ui(manager.getGroup(groupUI));

void Game::render() {
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_SetRenderTarget(ren, canva);
	float scaleX = 800.0f/WINDOW_WIDTH;
    float scaleY = 450.0f/WINDOW_HEIGHT;
    SDL_RenderSetScale(ren,1.0f,1.0f);
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
	
	SDL_SetRenderTarget(ren, NULL);
	SDL_RenderSetScale(ren,1.0f,1.0f);
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_RenderClear(ren);
	SDL_RenderCopy(ren, canva, NULL, NULL);
	SDL_RenderSetScale(ren,2.0f,2.0f);
	for(auto& u : ui) {
		u->draw();
	}
	SDL_RenderPresent(ren);
}

void Game::clean() {
	ResourceManager::clear();
	manager.clear();
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	IMG_Quit();
	TTF_Quit();
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