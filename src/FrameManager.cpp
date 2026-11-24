#include <SDL2/SDL.h>

#include "FrameManager.hpp"

int FrameManager::targetFPS = 0;
Uint64 FrameManager::frameStart = 0;
Uint64 FrameManager::frequency = 0;
Uint64 FrameManager::targetTicks = 0;
float FrameManager::deltaTime = 0.0f;
float FrameManager::frameTimes[10] = {};
int FrameManager::frameCount = 0;
float FrameManager::fps = 0.0f;

void FrameManager::init(int targetFPS) {
	FrameManager::targetFPS = targetFPS;
	frequency = SDL_GetPerformanceFrequency();
	targetTicks = (targetFPS > 0) ? (frequency / static_cast<Uint64>(targetFPS)) : 0;
	frameStart = SDL_GetPerformanceCounter();
	deltaTime = 0.0f;
	frameCount = 0;
	fps = 0.0f;
	for (int i = 0; i < 10; i++) {
		frameTimes[i] = 0.0f;
	}
}

float FrameManager::getDeltaTime() {
	return deltaTime;
}

float FrameManager::getFPS() {
	return fps;
}

bool FrameManager::frameDue() {
	Uint64 now = SDL_GetPerformanceCounter();
	Uint64 elapsed = now - frameStart;
	if (elapsed >= targetTicks) {
		deltaTime = static_cast<float>(elapsed) / static_cast<float>(frequency);
		frameStart = now;
		frameTimes[frameCount % 10] = deltaTime;
		frameCount++;
		int count = (frameCount > 10) ? 10 : frameCount;
		float sum = 0.0f;
		for (int i = 0; i < count; i++) {
			sum += frameTimes[i];
		}
		fps = (sum > 0.0f) ? (static_cast<float>(count) / sum) : 0.0f;
		return true;
	}
	return false;
}