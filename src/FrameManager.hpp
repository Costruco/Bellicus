#ifndef FRAMEMANAGER_HPP
#define FRAMEMANAGER_HPP

class FrameManager {
	private:
		static int targetFPS;
		static int trueWait;
		static int currentWait;
		static int wait;
		static int frameStart;
		static int deltaTime;
		
	public:
		static void init(int targetFPS);		
		static bool frameDue();
		
		static float getDeltaTime(); 
};

#endif
