#pragma once

#include <SDL2/SDL.h>

class FrameManager {
	private:
		static int targetFPS;
		static Uint64 frameStart;
		static Uint64 frequency;
		static Uint64 targetTicks;
		static float deltaTime;
		static float frameTimes[10];
		static int frameCount;
		static float fps;
		
	public:
		static void init(int targetFPS = 0);		
		static bool frameDue();
		
		static float getDeltaTime();
		static float getFPS();
};