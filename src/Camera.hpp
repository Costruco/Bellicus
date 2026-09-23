#pragma once

#include "Vector2D.hpp"

class Camera {
    public:
        float angle;
        Vector2D offset{};
        Vector2D scale;
        Vector2D focus{};
        Vector2D mouseOffset{};

        Camera() {
            angle = 0.0f;
			scale = Vector2D(1.0f,1.0f);
        }

        Vector2D worldToScreen(const Vector2D& world) const {
            Vector2D screen = world.rotate(focus,-angle);
            return (screen-focus)*this->scale+offset;
        }
		SDL_FRect worldToScreen(const SDL_FRect& world) const {
        	Vector2D pos(world.x,world.y), size(world.w,world.h);
        	pos = worldToScreen(pos);
        	size = worldSizeToScreen(size);
        	return {pos.x,pos.y,size.x,size.y};
		}

        Vector2D worldSizeToScreen(const Vector2D& worldSize) const {
            return worldSize*this->scale;
        }

        Vector2D screenToWorld(const Vector2D& screen) const {
            Vector2D world = screen.rotate(focus,angle);
            return (world-offset)/this->scale+focus;
        }

        Vector2D screenSizeToWorld(const Vector2D& screenSize) const {
            return screenSize/this->scale;
        }
};