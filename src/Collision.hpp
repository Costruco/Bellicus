#pragma once

#include "EntityComponentSystem/ColliderComponent.hpp"
#include "SDL2/SDL.h"
#include "Vector2D.hpp"
#include "Polygon.hpp"

class Collision {
	public:
		static bool SAT(const Polygon& a, const Polygon& b);
		static bool SAT(const ColliderComponent& a, const ColliderComponent& b);
};
