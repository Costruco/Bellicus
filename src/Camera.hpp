#pragma once

#include "Vector2D.hpp"

class Camera {
    public:
        float angle;
        Vector2D offset{};
        Vector2D scale{};
        Vector2D zoomCenter{};
        Vector2D mouseOffset{};

        Camera() {
            angle = 0.0f;
			scale = Vector2D(1.0f,1.0f);
        }

        void worldToScreen(const Vector2D& world, Vector2D& screen) {
            screen = (world-zoomCenter)*this->scale+offset;
            screen = screen.rotate(zoomCenter,angle);
        }
        void worldToScreen(Vector2D& world) {
        	worldToScreen(world,world);
		}
		void worldToScreen(const SDL_Rect& world, SDL_Rect& screen) {
        	Vector2D pos(world.x,world.y), size(world.w,world.h);
        	worldToScreen(pos);
        	worldSizeToScreen(size);
        	screen = {pos.x,pos.y,size.x,size.y};
		}

        void worldSizeToScreen(const Vector2D& worldSize, Vector2D& screenSize) {
            screenSize = worldSize*this->scale;
        }
        void worldSizeToScreen(Vector2D& screenSize) {
            worldSizeToScreen(screenSize,screenSize);
        }

        void screenToWorld(const Vector2D& screen, Vector2D& world) {
            world = screen.rotate(zoomCenter,-angle);
            world = (world-offset)/this->scale+zoomCenter;
        }
        void screenToWorld(Vector2D& screen) {
        	screenToWorld(screen,screen);
		}

        void screenSizeToWorld(const Vector2D& screenSize, Vector2D& worldSize) {
            worldSize = screenSize/this->scale;
        }
};