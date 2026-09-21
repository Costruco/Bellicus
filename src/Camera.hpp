#pragma once

#include "Vector2D.hpp"

class Camera {
    public:
        float angle;
        float xOffset, yOffset;
        float xScale, yScale;
        float xZoomCenter, yZoomCenter;
        float xMouseOffset, yMouseOffset;

        Camera() {
            angle = 0.0f;
            xOffset = 0.0f;
            yOffset = 0.0f;
            xScale = 1.0f;
            yScale = 1.0f;
            xZoomCenter = 0.0f;
            yZoomCenter = 0.0f;
            xMouseOffset = 0.0f;
            yMouseOffset = 0.0f;
        }

        void worldToScreen(float worldx, float worldy, float& screenx, float& screeny) {
            Vector2D screen = {worldx,worldy};
            screen = screen.rotate(Vector2D{xZoomCenter,yZoomCenter},angle);
            screen.x = (float)((screen.x-xZoomCenter)*xScale+xOffset);
            screen.y = (float)((screen.y-yZoomCenter)*yScale+yOffset);
            screenx = screen.x;
            screeny = screen.y;
        }

        void worldSizeToScreen(float worldw, float worldh, float& screenw, float& screenh) {
            screenw = worldw*xScale;
            screenh = worldh*yScale;
        }

        void screenToWorld(float screenx, float screeny, float& worldx, float& worldy) {
            Vector2D world;
            world.x = (float)((screenx-xOffset)/xScale+xZoomCenter);
            world.y = (float)((screeny-yOffset)/yScale+yZoomCenter);
            world = world.rotate(Vector2D{xZoomCenter,yZoomCenter},-angle);
            worldx = world.x;
            worldy = world.y;
        }

        void screenSizeToWorld(float screenw, float screenh, float& worldw, float& worldh) {
            worldw = screenw/xScale;
            worldh = screenh/yScale;
        }
};