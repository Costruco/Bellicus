#pragma once

#include "SDL2/SDL.h"
#include "Vector2D.hpp"

class Collision {
	public:
		static bool AABB(const SDL_FRect& a, const SDL_FRect& b);
		static bool SAT(const SDL_FRect& a, const SDL_FRect& b, float angleA, float angleB);
};
