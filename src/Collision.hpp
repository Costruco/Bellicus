#pragma once

#include "SDL2/SDL.h"
#include "Vector2D.hpp"
#include "Polygon.hpp"

class Collision {
	public:
		static bool SAT(Polygon a, Polygon b);
};
