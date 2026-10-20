#pragma once

#include <SDL2/SDL.h>

#include "EntityComponentSystem/ColliderComponent.hpp"
#include "Polygon.hpp"
#include "Vector2D.hpp"

class Collision {
	public:
		static bool SAT(const Polygon& a, const Polygon& b);
		static bool SAT(const ColliderComponent& a, const ColliderComponent& b);
};