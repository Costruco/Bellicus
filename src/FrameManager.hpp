#ifndef FRAMEMANAGER_HPP
#define FRAMEMANAGER_HPP

#include "SDL2/SDL.h"

class FrameManager {
	public:
		FrameManager(int targetFPS);
		~FrameManager();
		
		bool frameDue();
		
	private:
		int targetFPS,
			trueWait,
			currentWait,
			wait,
			frameStart;		
};

#endif
