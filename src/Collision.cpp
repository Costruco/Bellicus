#include "Collision.hpp"
#include "SDL2/SDL.h"

bool Collision::AABB(const SDL_FRect& a, const SDL_FRect& b) {
	if (a.x+a.w >= b.x && b.x+b.w >= a.x && a.y+a.h >= b.y && b.y+b.h >= a.y) {
		return true;
	}
	return false;
}

