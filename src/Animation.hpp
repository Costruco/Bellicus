#pragma once

struct Animation {
	int index;
	int frames;
	int frameDelay;
	
	Animation() {}
	Animation(int index, int frames, int frameDelay) {
		this->index = index;
		this->frames = frames;
		this->frameDelay = frameDelay; 
	}
};
