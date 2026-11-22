#pragma once

#include "Utils/Vector2D.hpp"

class Entity;

class Camera {
    public:
        float angle;
        Vector2D offset;
        Vector2D scale;
        Vector2D focus;
        Vector2D mouseOffset;
        Entity * target = nullptr;
        bool followAngle = false;
        int mapSizeX = 0;
        int mapSizeY = 0;
        int maxAudioDistance = 1000;

        Camera() : angle(0.0f), offset(), scale(1.0f,1.0f), focus(), mouseOffset() {}

        Vector2D worldToScreen(const Vector2D& world) const {
            return ((world-focus)*scale).rotate(Vector2D(),-angle)+offset;
        }

		SDL_FRect worldToScreen(const SDL_FRect& world) const {
        	Vector2D pos(world.x,world.y), size(world.w,world.h);
        	pos = worldToScreen(pos);
        	size = worldSizeToScreen(size);
        	return {pos.x,pos.y,size.x,size.y};
		}

        Vector2D worldSizeToScreen(const Vector2D& worldSize) const {
            return worldSize*scale;
        }

        Vector2D screenToWorld(const Vector2D& screen) const {
            return (screen-offset).rotate(Vector2D(),angle)/scale+focus;
        }

        Vector2D screenSizeToWorld(const Vector2D& screenSize) const {
            return screenSize/scale;
        }

        void setTarget(Entity * newTarget, bool followDirection = false) {
            target = newTarget;
            followAngle = followDirection;
        }

        void setTarget(Entity& newTarget, bool followDirection = false) {
            target = &newTarget;
            followAngle = followDirection;
        }

        void setMapSize(int sizeX, int sizeY) {
            mapSizeX = sizeX;
            mapSizeY = sizeY;
        }

        void setMaxAudioDistance(int distance) {
            maxAudioDistance = distance;
        }

        void update();

        void update(int sizeX, int sizeY) {
            setMapSize(sizeX, sizeY);
            update();
        }
};