#include "FrameManager.hpp"

int FrameManager::targetFPS = 0;
int FrameManager::trueWait = 0;
int FrameManager::currentWait = 0;
int FrameManager::wait = 0;
int FrameManager::frameStart = 0;
int FrameManager::deltaTime = 0;

void FrameManager::init(int targetFPS) {
	FrameManager::targetFPS = targetFPS;
	trueWait = currentWait = 1000000/targetFPS;
	wait = currentWait/1000;
	frameStart = SDL_GetTicks();
}

int FrameManager::getDt() {
	return deltaTime;
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
