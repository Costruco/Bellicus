#ifndef FRAMEMANAGER_HPP
#define FRAMEMANAGER_HPP

#include "Game.hpp"

class FrameManager {
	public:
		static void init(int targetFPS);		
		static bool frameDue();
		
		static int getDt(); 
		
	private:
		static int targetFPS;
		static int trueWait;
		static int currentWait;
		static int wait;
		static int frameStart;	
		static int deltaTime;	
};

#endif
