#include <SDL2/SDL.h>

#include "FrameManager.hpp"

int FrameManager::targetFPS = 0;
Uint64 FrameManager::frameStart = 0;
Uint64 FrameManager::frequency = 0;
Uint64 FrameManager::targetTicks = 0;
float FrameManager::deltaTime = 0.0f;

void FrameManager::init(int targetFPS) {
	FrameManager::targetFPS = targetFPS;
	frequency = SDL_GetPerformanceFrequency();
	targetTicks = (targetFPS > 0) ? (frequency / static_cast<Uint64>(targetFPS)) : 0;
	frameStart = SDL_GetPerformanceCounter();
	deltaTime = 0.0f;
}

float FrameManager::getDeltaTime() {
	return deltaTime;
}

bool FrameManager::frameDue() {
	Uint64 now = SDL_GetPerformanceCounter();
	Uint64 elapsed = now - frameStart;
	if (elapsed >= targetTicks) {
		deltaTime = static_cast<float>(elapsed) / static_cast<float>(frequency);
		frameStart = now;
		return true;
	}
	return false;
}