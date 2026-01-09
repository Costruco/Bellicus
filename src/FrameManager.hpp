#ifndef FRAMEMANAGER_HPP
#define FRAMEMANAGER_HPP

#include "Game.hpp"

class FrameManager {
	public:
		static void init(int targetFPS);		
		static bool frameDue();
		
		static int getDeltaTime(); 
		
	private:
		static int targetFPS,
				   trueWait,
				   currentWait,
				   wait,
				   frameStart,	
		           deltaTime;
};

#endif
