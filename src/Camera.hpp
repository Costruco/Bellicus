#pragma once

class Camera {
    public:
        float xoffset, yoffset;
        float xscale, yscale;
        float xmouseoffset, ymouseoffset;

        Camera() {
            xoffset = 0.0f;
            yoffset = 0.0f;
            xscale = 1.0f;
            yscale = 1.0f;
            xmouseoffset = 0.0f;
            ymouseoffset = 0.0f;
        }

        void worldToScreen(float worldx, float worldy, float& screenx, float& screeny) {
            screenx = (float)(int)((worldx-(xoffset+xmouseoffset))*xscale);
            screeny = (float)(int)((worldy-(yoffset+ymouseoffset))*yscale);
        }
        void screenToWorld(float screenx, float screeny, float& worldx, float& worldy) {
            worldx = (float)(screenx/xscale)+xoffset+xmouseoffset;
            worldy = (float)(screeny/yscale)+yoffset+ymouseoffset;
        }
};