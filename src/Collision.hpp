#pragma once

#include "SDL2/SDL.h"

class Collision {
	public:
		static bool AABB(const SDL_FRect& a, const SDL_FRect& b);
};
