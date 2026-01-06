#include "FrameManager.hpp"

FrameManager::FrameManager(int targetFPS) {
	this->targetFPS = targetFPS;
	trueWait = currentWait = 1000000/targetFPS;
	wait = currentWait/1000;
	frameStart = SDL_GetTicks();
}

FrameManager::~FrameManager() {
	
}

bool FrameManager::frameDue() {
	if (SDL_GetTicks()-frameStart >= wait) {
		frameStart = SDL_GetTicks();
		currentWait += trueWait-wait*1000;
		wait = currentWait/1000;
		return true;
	} else
		return false;
}
