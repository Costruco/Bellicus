#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "Components.hpp"

class ProgressBar : public Component {
private:
    TransformComponent * transform;
    float progressValue = 0.0f;
    float capacityValue = 1.0f;
    SDL_Color progressColor = {0,255,0,255};
    SDL_Color backgroundColor = {255,0,0,255};

public:
    ProgressBar(float progressValue, float capacityValue) : 
        transform(nullptr), 
        progressValue(progressValue),
        capacityValue(capacityValue) {}

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    void draw() override {
        if (transform) {
            float barHeight = transform->height;
            float barWidth = transform->width;
            float progress = barWidth*(progressValue/capacityValue);
            float xpos = transform->position.x-barWidth/2.0f;
            float ypos = transform->position.y-barHeight/2.0f;
            SDL_Color * p = &progressColor;
            SDL_Color * b = &backgroundColor;
            boxRGBA(Game::ren,xpos,ypos,xpos+barWidth,ypos+barHeight,b->r,b->g,b->b,b->a);
            boxRGBA(Game::ren,xpos,ypos,xpos+progress,ypos+barHeight,p->r,p->g,p->b,p->a);
        }
    }

    void setProgress(float progress) {
        progressValue = progress;
        if (progressValue > capacityValue) {
            progressValue = capacityValue;
        } else if (progressValue < 0.0f) {
            progressValue = 0.0f;
        }
    }

    void setCapacity(float capacity) {
        capacityValue = capacity;
        if (capacityValue < 0.0f) {
            capacityValue = 0.0f;
        }
    }

    float getProgress() const {
        return progressValue;
    }

    float getCapacity() const {
        return capacityValue;
    }
};