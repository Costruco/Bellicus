#pragma once

#include <SDL2/SDL.h>

#include <algorithm>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Components.hpp"
#include "FrameManager.hpp"
#include "Game.hpp"
#include "ResourceManager.hpp"
#include "Utils/Math.hpp"
#include "Utils/Timer.hpp"
#include "Utils/Vector2D.hpp"

struct ParticleSpec {
    float lifetime = 0.0f;
    float speed = 0.0f;
    float scale = 1.0f;
    float spreadAngle = 0.0f;
    float spawnRadius = 0.0f;
    Vector2D centerOffset;
    int frames = 1;
    int columns = 0;
    bool randomRotation = false;
    bool followEmitter = false;
    ResourceManager::TexturePtr texture;
    std::unordered_map<std::string, ResourceManager::SoundPtr> sound;
};

class ParticleComponent : public Component {
private:
    TransformComponent * transform = nullptr;
    SoundComponent * sound = nullptr;
    inline static std::unordered_map<std::string, ParticleSpec> specs;
    const ParticleSpec * spec = nullptr;
    Timer lifetimeTimer;

public:
    ParticleComponent(const std::string& specID) :
        transform(nullptr),
        sound(nullptr),
        spec(getSpec(specID)),
        lifetimeTimer(spec ? static_cast<Uint32>(spec->lifetime) : 0) {}

    static ParticleSpec * getSpec(const std::string& id) {
        auto it = specs.find(id);
        if (it != specs.end()) {
            return &it->second;
        }
        return nullptr;
    }

    static void addSpec(const std::string& id, const std::string& texturePath, float lifetime,
        int frames = 1, int columns = 0, float scale = 1.0f, Vector2D centerOffset = Vector2D(),
        float speed = 0.0f, float spreadAngle = 0.0f, float spawnRadius = 0.0f,
        bool randomRotation = false, bool followEmitter = false,
        const std::vector<std::pair<std::string, std::string>>& soundFiles = {}, bool isLinear = false) {
        ParticleSpec newSpec;
        newSpec.lifetime = lifetime;
        newSpec.speed = speed;
        newSpec.scale = scale;
        newSpec.spreadAngle = spreadAngle;
        newSpec.spawnRadius = spawnRadius;
        newSpec.centerOffset = centerOffset;
        newSpec.frames = std::max(1, frames);
        newSpec.columns = columns;
        newSpec.randomRotation = randomRotation;
        newSpec.followEmitter = followEmitter;
        newSpec.texture = ResourceManager::loadTexture(texturePath, isLinear);
        for (const auto& pair : soundFiles) {
            newSpec.sound[pair.first] = ResourceManager::loadSound(pair.second);
        }
        specs[id] = newSpec;
    }

    static void clearSpecs() {
        specs.clear();
    }

    static Entity * spawn(const std::string& specID, Vector2D offsetOrPos = Vector2D(),
        float angleOffsetOrAngle = 0.0f, Entity * emitter = nullptr, int overrideFollowEmitter = -1) {
        const ParticleSpec * spec = getSpec(specID);
        if (!spec)
            return nullptr;

        auto randUnit = []() -> float {
            return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        };

        float spread = (spec->spreadAngle > 0.0f)
            ? (randUnit() * 2.0f - 1.0f) * spec->spreadAngle
            : 0.0f;
        float jitterDist = (spec->spawnRadius > 0.0f)
            ? randUnit() * spec->spawnRadius
            : 0.0f;

        int texW = 0, texH = 0;
        if (spec->texture) {
            SDL_QueryTexture(spec->texture.get(), nullptr, nullptr, &texW, &texH);
        }
        int cols = (spec->columns > 0) ? spec->columns : std::max(1, spec->frames);
        int rows = (spec->columns > 0 && spec->frames > 0)
            ? ((spec->frames + spec->columns - 1) / spec->columns)
            : 1;
        float frameW = static_cast<float>(texW / std::max(1, cols));
        float frameH = static_cast<float>(texH / std::max(1, rows));

        bool attach = (overrideFollowEmitter >= 0)
            ? (overrideFollowEmitter != 0)
            : spec->followEmitter;
        attach = attach && (emitter != nullptr);

        Entity& particle = manager.addEntity();

        if (attach) {
            float localMoveAngle = clockLimit(angleOffsetOrAngle + spread, 0.0f, 360.0f);
            Vector2D localPos = offsetOrPos + Vector2D::fromPolar(localMoveAngle, jitterDist);
            float localVisualAngle = spec->randomRotation ? (randUnit() * 360.0f) : localMoveAngle;

            auto& t = particle.addComponent<TransformComponent>(
                localPos, localVisualAngle, frameW, frameH, spec->scale, spec->centerOffset);
            t.velocity = Vector2D::fromPolar(localMoveAngle, spec->speed);
            particle.setFather(emitter);
        } else {
            Vector2D worldPos = offsetOrPos;
            float worldMoveAngle = clockLimit(angleOffsetOrAngle + spread, 0.0f, 360.0f);
            float worldScale = spec->scale;

            if (emitter && emitter->hasComponent<TransformComponent>()) {
                const auto& emitterTransform = emitter->getComponent<TransformComponent>();
                float emitterAngle = emitterTransform.getDirection();
                float emitterScale = emitterTransform.getScale();
                worldMoveAngle = clockLimit(emitterAngle + angleOffsetOrAngle + spread, 0.0f, 360.0f);
                Vector2D rotatedOffset = (offsetOrPos * emitterScale).rotate({0, 0}, emitterAngle);
                worldPos = emitterTransform.getPosition() + rotatedOffset +
                           Vector2D::fromPolar(worldMoveAngle, jitterDist * emitterScale);
                worldScale = spec->scale * emitterScale;
            } else {
                worldPos = offsetOrPos + Vector2D::fromPolar(worldMoveAngle, jitterDist);
            }

            float worldVisualAngle = spec->randomRotation ? (randUnit() * 360.0f) : worldMoveAngle;
            auto& t = particle.addComponent<TransformComponent>(
                worldPos, worldVisualAngle, frameW, frameH, worldScale, spec->centerOffset);
            t.velocity = Vector2D::fromPolar(worldMoveAngle, spec->speed);
        }

        int totalFrames = std::max(1, spec->frames);
        int frameDelay = (totalFrames > 0 && spec->lifetime > 0.0f)
            ? std::max(1, static_cast<int>(spec->lifetime / static_cast<float>(totalFrames)))
            : 100;

        particle.addComponent<SpriteComponent>(spec->texture, totalFrames, frameDelay, false, spec->columns);
        particle.addComponent<ParticleComponent>(specID);
        particle.addGroup(groupParticles);
        return &particle;
    }

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
        if (spec && !spec->sound.empty()) {
            if (!entity->hasComponent<SoundComponent>())
                sound = &entity->addComponent<SoundComponent>();
            else
                sound = &entity->getComponent<SoundComponent>();
            sound->addSounds(spec->sound);
            sound->playSound("spawn");
        }

        if (spec && spec->texture && (transform->width == 0.0f || transform->height == 0.0f)) {
            int w = 0, h = 0;
            SDL_QueryTexture(spec->texture.get(), nullptr, nullptr, &w, &h);
            int cols = (spec->columns > 0) ? spec->columns : std::max(1, spec->frames);
            int rows = (spec->columns > 0 && spec->frames > 0)
                ? ((spec->frames + spec->columns - 1) / spec->columns)
                : 1;
            if (transform->width == 0.0f)
                transform->width = static_cast<float>(w / std::max(1, cols));
            if (transform->height == 0.0f)
                transform->height = static_cast<float>(h / std::max(1, rows));
        }
    }

    void update() override {
        if (!spec)
            return;
        float dt = FrameManager::getDeltaTime();
        if (dt > 0.0f && transform) {
            transform->acceleration = Vector2D();
            transform->position += transform->velocity * dt;
        }
        if (!lifetimeTimer.isRunning()) {
            if (sound)
                sound->playSound("destruction");
            this->entity->destroy();
        }
    }
};
