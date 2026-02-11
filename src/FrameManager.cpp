#include "FrameManager.hpp"

#include "SDL2/SDL.h"

int FrameManager::targetFPS = 0,
	FrameManager::trueWait = 0,
	FrameManager::currentWait = 0,
    FrameManager::wait = 0,
    FrameManager::frameStart = 0,
    FrameManager::deltaTime = 0;

void FrameManager::init(int targetFPS) {
	FrameManager::targetFPS = targetFPS;
	trueWait = currentWait = 1000000/targetFPS;
	wait = currentWait/1000;
	frameStart = SDL_GetTicks();
}

float FrameManager::getDeltaTime() {
	return static_cast<float>(deltaTime)/1000;
}

bool FrameManager::frameDue() {
	int dt = SDL_GetTicks()-frameStart;
	if (dt >= wait) {
		deltaTime = dt;
		frameStart = SDL_GetTicks();
		currentWait += trueWait-wait*1000;
		wait = currentWait/1000;
		return true;
	} else
		return false;
}
