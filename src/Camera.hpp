#pragma once

class Camera {
    public:
        float xOffset, yOffset;
        float xScale, yScale;
        float xZoomCenter, yZoomCenter;
        float xMouseOffset, yMouseOffset;

        Camera() {
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
            screenx = (float)((worldx-xZoomCenter)*xScale+xOffset);
            screeny = (float)((worldy-yZoomCenter)*yScale+yOffset);
        }
        void screenToWorld(float screenx, float screeny, float& worldx, float& worldy) {
            worldx = (float)((screenx-xOffset)/xScale+xZoomCenter);
            worldy = (float)((screeny-yOffset)/yScale+yZoomCenter);
        }
};