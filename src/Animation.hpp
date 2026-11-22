#pragma once

struct Animation {
	int index = 0;
	int frames = 1;
	int frameDelay = 100;
	bool loop = true;
	int columns = 0;
	
	Animation() = default;
	Animation(int index, int frames, int frameDelay, bool loop = true, int columns = 0) {
		this->index = index;
		this->frames = frames;
		this->frameDelay = frameDelay;
		this->loop = loop;
		this->columns = columns;
	}
};