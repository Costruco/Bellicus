#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <vector>

#include "Camera.hpp"
#include "Utils/Collision.hpp"
#include "Utils/Timer.hpp"
#include "EntityComponentSystem/Components.hpp"
#include "FrameManager.hpp"
#include "Game.hpp"
#include "Map.hpp"
#include "ResourceManager.hpp"
#include "Utils/Vector2D.hpp"

Manager manager;

SDL_Renderer * Game::ren = nullptr;
SDL_Texture * Game::canva = nullptr;
int Game::WINDOW_WIDTH = 0;
int Game::WINDOW_HEIGHT = 0;
SDL_Event Game::evt;
std::vector<ColliderComponent*> Game::colliders;
const Uint8* Game::keystate = nullptr;
Camera Game::camera;
Vector2D mouse;
Timer reload = Timer(1000);
	
auto& tankChassis(manager.addEntity());
auto& tankTurret(manager.addEntity());
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
	
	manager.setComponentEnable<ColliderComponent>(false,EnableMode::DRAW);
	Map::loadMap("../assets/maps/map1.map",mapSizeX,mapSizeY);
	manager.setGroupEnable(groupMap, false, EnableMode::UPDATE);
	

	//inicializa entidades
	tankChassis.addComponent<TransformComponent>(0,0,0.0f,203,104,1);
	tankChassis.addComponent<SpriteComponent>("../assets/textures/entities/T-34/chassi_com_sombra.png",false,true);
	tankChassis.addComponent<ColliderComponent>("player",195,96);
	tankChassis.addComponent<TankMovementComponent>(0,54.0f,370000,36250,94000,0.15,6,std::vector<Vector2D>{{-65,0},{0,0},{0,65},{65,135},{135,225},{225,487}});
	tankChassis.addComponent<KeyboardController>();
	tankChassis.addGroup(groupPlayers);
	
	tankTurret.addComponent<TransformComponent>(19,0,0.0f,146,68,1,-37,0);
	tankTurret.addComponent<SpriteComponent>("../assets/textures/entities/T-34/torre_com_sombra.png",false,true);
	tankTurret.addComponent<ColliderComponent>("player",138,60);
	tankTurret.addGroup(groupPlayers);

	tankTurret.setFather(&tankChassis);

	wall.addComponent<TransformComponent>(200,200,45.0f,200,300,1);
	wall.addComponent<SpriteComponent>("../assets/textures/entities/brick_wall.png");
	wall.addComponent<ColliderComponent>("wall");
	wall.addGroup(groupMap);

	vaga.addComponent<TransformComponent>(-200,-200,0.0f,637,90,1);
	vaga.addComponent<SpriteComponent>("../assets/textures/features/vaga.png");
	vaga.addGroup(groupGround);

	info.addComponent<MessageBoxComponent>("../assets/fonts/sdl2_gfx_8x8.ttf",8,
											std::vector<std::string>{
											"x,y: /right[:1.0f],[:1.0f]",
											"Angulo: /right[:1.2f]",
											"Velocidade: /right[:5.1f]",
											"Gear: /right[:1.0f]"},
											ScreenAlignment::TOP_LEFT);
	info.getComponent<MessageBoxComponent>().setColor({255,255,255,255},{255,255,255,255},{0,0,0,255});
	info.addGroup(groupUI);
}

void Game::handleEvents() {
	keystate = SDL_GetKeyboardState(NULL);

	int mouseX,mouseY;
	SDL_GetMouseState(&mouseX,&mouseY);
	mouse = {mouseX,mouseY};

	if (tankTurret.isActive() && tankTurret.isComponentEnabled<TransformComponent>(EnableMode::UPDATE)) {
		auto turret = &tankTurret.getComponent<TransformComponent>();
		auto chassis = &tankChassis.getComponent<TransformComponent>();

		double targetAngle = angleBetweenPoints(turret->getPosition(),camera.screenToWorld(mouse*logicalScale));
		turret->direction = 180+targetAngle-chassis->getDirection();
	}
	
	while (SDL_PollEvent(&evt)) {
		switch (evt.type) {
			case SDL_KEYDOWN:
				if (evt.key.keysym.sym == SDLK_ESCAPE)
					camera.mouseOffset = Vector2D();
				if (evt.key.keysym.sym == SDLK_F1)
					manager.setComponentEnable<ColliderComponent>(
					!manager.isComponentEnabled<ColliderComponent>(EnableMode::DRAW), EnableMode::DRAW);
				break;
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_MOUSEWHEEL:
				if (evt.wheel.y > 0 && camera.scale.x+0.1f <= 2.0f)
					camera.scale += {0.1f,0.1f};
				else if (evt.wheel.y < 0 && camera.scale.x-0.1f >= 0.5f)
					camera.scale -= {0.1f,0.1f};
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (evt.button.button == SDL_BUTTON_LEFT && reload.ifReset()) {
				}
				break;
			default:
				break;
		}
	}
}

void Game::update() {
	manager.refresh();
	manager.update();

	auto transform = tankChassis.getComponent<TransformComponent>();
	auto tank = tankChassis.getComponent<TankMovementComponent>();
	float speed = tank.speed/PIXELS_PER_METER*3.6f;
	float gear = tank.currentGear;
	info.getComponent<MessageBoxComponent>().setValues(std::vector<std::vector<float>>{
													   {transform.getPosition().x,transform.getPosition().y},
													   {transform.getDirection()},
													   {speed},
													   {gear}});
	
	camera.offset = {WINDOW_WIDTH/2,WINDOW_HEIGHT/2};
	camera.angle = 90+tankChassis.getComponent<TransformComponent>().getDirection();
	SDL_FPoint playerPos = tankChassis.getComponent<TransformComponent>().getPosition();

	//destroi o carro para testar fim de jogo e destruicao de entidades
	if (playerPos.x > 800) {
		tankChassis.destroy();
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
	
	updateCounter++;

	if (updateCounter == 10) {
		//std::cout << Vector2D(playerPos) << std::endl;
		//std::cout << tankChassis.getComponent<TransformComponent>().getDirection() << std::endl;
		//std::cout << "---------------------------------------------------------------------------------------" << std::endl;
		//std::cout << tankChassis.getComponent<CarMovementComponent>().velocity.getModule()/PIXELS_PER_METER*3.6f << "Km/h" << std::endl;
		//std::cout << tankChassis.getComponent<CarMovementComponent>().gearbox.gear << std::endl;
		//std::cout << tankChassis.getComponent<CarMovementComponent>().engineRPM << std::endl;
	}
	
	for (auto c : colliders) {
		if (Collision::SAT(tankChassis.getComponent<ColliderComponent>(),*c)) {
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
	boxRGBA(ren,mouse.x,mouse.y,mouse.x+100,mouse.y+10,255,0,0,255);
	boxRGBA(ren,mouse.x,mouse.y,mouse.x+reload.progress()*100,mouse.y+10,0,255,0,255);
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