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
auto& tankCupola(manager.addEntity());
auto& tankMachineGun(manager.addEntity());
auto& wall(manager.addEntity());
auto& vaga(manager.addEntity());
auto& info(manager.addEntity());
auto& reloadBar(manager.addEntity());

int mapSizeX,mapSizeY;
const std::string mapfile = "../assets/textures/tiles/tile_map_1.png";

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
		Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) == (MIX_INIT_MP3 | MIX_INIT_OGG)) {
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
		Mix_AllocateChannels(32);
		Mix_Volume(-1, 10);
		Mix_VolumeMusic(10);
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
	camera.setMapSize(mapSizeX,mapSizeY);
	manager.setGroupEnable(groupMap, false, EnableMode::UPDATE);
	
	//inicializa especificacoes de particulas, geradores, projeteis e armas
	ParticleComponent::addSpec("exhaust_smoke",
		"../assets/textures/particles/fumaca.png",
		200.0f, 8, 8, 0.5f, {0.0f, 0.0f}, 150.0f, 10.0f, 12.0f, true, false);
	ParticleComponent::addSpec("cannon_blast",
		"../assets/textures/particles/blast.png",
		400.0f, 25, 5, 1.0f, {-90.0f, 0.0f}, 0.0f, 0.0f, 0.0f, false, true);
	ParticleComponent::addSpec("coaxial_mg_flash",
		"../assets/textures/particles/flash_metralhadora_coaxial.png",
		16.0f, 1, 1, 1.0f, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, false, true);
	ParticleComponent::addSpec("hull_mg_flash",
		"../assets/textures/particles/flash_metralhadora_chassi.png",
		16.0f, 1, 1, 1.0f, {-2.5f, -0.5f}, 0.0f, 0.0f, 0.0f, false, true);
	ParticleComponent::addSpec("cupola_flash",
		"../assets/textures/particles/flash_cupola_chassi.png",
		16.0f, 1, 1, 1.0f, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, false, true);
	ParticleComponent::addSpec("cannon_explosion",
		"../assets/textures/particles/explosao.png",
		1000.0f, 25, 5, 2.0f, {0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, false, false);

	EntityGeneratorComponent::addSpec("exhaust_smoke_gen", "exhaust_smoke", 7, -1, true, false);

	ProjectileComponent::addSpec("cannon_shell",800.0f,1200.0f,3000.0f,
		"../assets/textures/projectiles/municao.png",
		{
			{"destruction","../assets/sound/bala_destruida.mp3"}
		},
		false, "cannon_explosion");
	ProjectileComponent::addSpec("mg_bullet",1000.0f,900.0f,2000.0f,
		"../assets/textures/projectiles/municao_metralhadora.png",
		{
			{"destruction","../assets/sound/bala_destruida.mp3"}
		});
	ProjectileComponent::addSpec("soldier_bullet",600.0f,700.0f,2000.0f,
		"../assets/textures/projectiles/balaSoldado.png",
		{
			{"destruction","../assets/sound/bala_destruida.mp3"}
		});
	WeaponComponent::addSpec("tank_cannon",1,0.5f,2000,1.0f,"cannon_shell",
		{
			{"fire","../assets/sound/disparo_canhao.ogg"},
			{"reload","../assets/sound/recarregamento.mp3"}
		},
		"cannon_blast");
	WeaponComponent::addSpec("machine_gun",50,2.5f,3000,10.0f,"mg_bullet",
		{
			{"fire","../assets/sound/disparo_metralhadora.ogg"},
			{"reload","../assets/sound/recarregamento.mp3"}
		},
		"coaxial_mg_flash");

	//inicializa entidades
	//tanque
	tankChassis.addComponent<TransformComponent>(0,0,0.0f,203,104,1);
	tankChassis.addComponent<SpriteComponent>("../assets/textures/entities/T-34/chassi_com_sombra.png",false,true);
	tankChassis.addComponent<ColliderComponent>("player",195,96);
	tankChassis.addComponent<TankMovementComponent>(54.0f,370000,36250,94000,0.095f,6,std::vector<Vector2D>{{-100,0},{0,0},{0,100},{100,208},{208,346},{346,750}});
	tankChassis.addComponent<KeyboardController>();
	auto& tankExhaust = tankChassis.addComponent<EntityGeneratorComponent>();
	tankExhaust.addGenerator("exhaust_smoke_gen", {-93.0f, -15.0f}, 180.0f);
	tankExhaust.addGenerator("exhaust_smoke_gen", {-93.0f, 15.0f}, 180.0f);
	tankChassis.addGroup(groupPlayers);
	camera.setTarget(tankChassis);

	tankMachineGun.addComponent<TransformComponent>(64.5f,18.0f,0.0f,14,8,1,-4,0);
	tankMachineGun.addComponent<SpriteComponent>("../assets/textures/entities/T-34/metralhadora_chassi.png",false,true);
	tankMachineGun.addComponent<ColliderComponent>("player",14,8);
	auto& machineGun = tankMachineGun.addComponent<WeaponComponent>();
	machineGun.addGun("machine_gun", {11.0f, 0.0f}, 0.0f, -1, "hull_mg_flash");
	tankMachineGun.setFather(&tankChassis);
	tankMachineGun.addGroup(groupPlayers);

	tankCupola.addComponent<TransformComponent>(62.0f,18.0f,0.0f,11,12,1);
	tankCupola.addComponent<SpriteComponent>("../assets/textures/entities/T-34/cupola_metralhadora_chassi.png",false,true);
	tankCupola.addComponent<ColliderComponent>("player",11,12);
	tankCupola.setFather(&tankChassis);
	tankCupola.addGroup(groupPlayers);
	
	tankTurret.addComponent<TransformComponent>(19,0,0.0f,146,68,1,-37,0);
	tankTurret.addComponent<SpriteComponent>("../assets/textures/entities/T-34/torre_com_sombra.png",false,true);
	tankTurret.addComponent<ColliderComponent>("player",138,60);
	auto& turretWeapons = tankTurret.addComponent<WeaponComponent>();
	turretWeapons.addGun("tank_cannon", {105.0f, 0.0f});
	turretWeapons.addGun("machine_gun", {42.0f, 7.5f});
	tankTurret.setFather(&tankChassis);
	tankTurret.addGroup(groupPlayers);
	//fim do tanque

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
											ScreenAlignment::TOP_CENTER,2.0f);
	info.getComponent<MessageBoxComponent>().setColor({255,255,255,255},{255,255,255,255},{0,0,0,255});
	info.addGroup(groupUI);

	reloadBar.addComponent<TransformComponent>(0,0,0.0f,100,20);
	reloadBar.addComponent<ProgressBar>(0,100);
	reloadBar.addGroup(groupUI);
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

	const auto& transform = tankChassis.getComponent<TransformComponent>();
	const auto& tank = tankChassis.getComponent<TankMovementComponent>();
	float speed = transform.getForwardSpeed()/PIXELS_PER_METER*3.6f;
	float gear = tank.currentGear;
	info.getComponent<MessageBoxComponent>().setValues(std::vector<std::vector<float>>{
													   {transform.getPosition().x,transform.getPosition().y},
													   {transform.getDirection()},
													   {speed},
													   {gear}});

	if (tankTurret.isActive() && tankTurret.hasComponent<WeaponComponent>()) {
		auto& weapons = tankTurret.getComponent<WeaponComponent>();
		weapons.shootAll();
		if (!weapons.gunSlots.empty()) {
			const auto& cannon = weapons.gunSlots[0];
			float progress = cannon.reloading ? cannon.reloadTimer.progress() : cannon.fireTimer.progress();
			reloadBar.getComponent<ProgressBar>().setProgress(progress*reloadBar.getComponent<ProgressBar>().getCapacity());
		}
	}
	reloadBar.getComponent<TransformComponent>().position = {mouse.x,mouse.y};

	//destroi o carro para testar fim de jogo e destruicao de entidades
	if (transform.getPosition().x > 800) {
		tankChassis.destroy();
		manager.refresh();
		Game::render();
		SDL_Delay(1000);
		isRunning = false;
		return;
	}

	camera.update();
	
	updateCounter++;
	
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
auto& projectiles(manager.getGroup(groupProjectiles));
auto& particles(manager.getGroup(groupParticles));
auto& ui(manager.getGroup(groupUI));

void Game::render() {
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_SetRenderTarget(ren, canva);
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
	for (auto& pr : projectiles) {
		pr->draw();
	}
	for (auto& pt : particles) {
		pt->draw();
	}

	//desenhar colisoes SE debug
	if (manager.isComponentEnabled<ColliderComponent>(EnableMode::DRAW)) {
		for (auto* c : colliders)
			if (c->entity->isActive() && c->entity->isComponentEnabled<ColliderComponent>(EnableMode::DRAW))
				c->drawDebug();
	}
	
	SDL_SetRenderTarget(ren, NULL);
	SDL_SetRenderDrawColor(ren,0,0,0,255);
	SDL_RenderClear(ren);
	SDL_RenderCopy(ren, canva, NULL, NULL);
	for(auto& u : ui) {
		u->draw();
	}
	SDL_RenderPresent(ren);
}

void Game::clean() {
	EntityGeneratorComponent::clearSpecs();
	ParticleComponent::clearSpecs();
	ProjectileComponent::clearSpecs();
	WeaponComponent::clearSpecs();
	ResourceManager::clear();
	manager.clear();
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	Mix_Quit();
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