#pragma once

#include <SDL2/SDL.h>

#include <string>
#include <unordered_map>

#include "Components.hpp"
#include "FrameManager.hpp"
#include "ResourceManager.hpp"
#include "Utils/Timer.hpp"
#include "Utils/Vector2D.hpp"

struct ProjectileSpec {
    float speed;
    float range;
    float lifetime;
    ResourceManager::TexturePtr texture;
};

class ProjectileComponent : public Component {
private:
    TransformComponent * transform;
    inline static std::unordered_map<std::string,ProjectileSpec> specs;
    const ProjectileSpec * spec;
    float distanceTraveled;
    Timer lifetimeTimer;
    
public:
    ProjectileComponent(const std::string& specID) :
        transform(nullptr),
        spec(getSpec(specID)),
        distanceTraveled(0.0f),
        lifetimeTimer(spec ? static_cast<Uint32>(spec->lifetime) : 0) {}

    static ProjectileSpec * getSpec(const std::string& id) {
        auto it = specs.find(id);
        if (it != specs.end()) {
            return &it->second;
        }
        return nullptr;
    }

    static void addSpec(const std::string& id, float speed, float range, float lifetime, const std::string& texturePath, bool isLinear = false) {
        ProjectileSpec newSpec;
        newSpec.speed = speed;
        newSpec.range = range;
        newSpec.lifetime = lifetime;
        if (isLinear)
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
        newSpec.texture = ResourceManager::loadTexture(texturePath);
        if (isLinear)
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        specs[id] = newSpec;
    }

    static void clearSpecs() {
        specs.clear();
    }

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
        if (spec && spec->texture && (transform->width == 0.0f || transform->height == 0.0f)) {
            int w = 0, h = 0;
            SDL_QueryTexture(spec->texture.get(), nullptr, nullptr, &w, &h);
            if (transform->width == 0.0f)
                transform->width = static_cast<float>(w);
            if (transform->height == 0.0f)
                transform->height = static_cast<float>(h);
        }
    }

    void update() override {
        if (!spec)
            return;
        float dt = FrameManager::getDeltaTime();
        Vector2D velocity = Vector2D::fromPolar(transform->getDirection(), spec->speed);
        Vector2D step = velocity * dt;
        transform->position += step;
        distanceTraveled += step.getModule();
        if (distanceTraveled >= spec->range || !lifetimeTimer.isRunning()) {
            this->entity->destroy();
        }
    }
};
