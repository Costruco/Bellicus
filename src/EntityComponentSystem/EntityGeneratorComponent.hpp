#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Components.hpp"
#include "ParticleComponent.hpp"
#include "ResourceManager.hpp"
#include "Utils/Timer.hpp"
#include "Utils/Vector2D.hpp"

using EntitySpawnFactory = std::function<Entity*(Entity * generatorEntity, Vector2D offset, float angleOffset, bool attachToGenerator)>;

struct GeneratorSpec {
    std::string targetID;
    Uint32 interval = 0;
    int maxGenerations = -1;
    bool autoGenerate = true;
    bool attachToEntity = false;
    EntitySpawnFactory customFactory = nullptr;
    std::unordered_map<std::string, ResourceManager::SoundPtr> sound;
};

struct GeneratorSlot {
    std::string specID;
    Vector2D offset;
    float angleOffset = 0.0f;
    const GeneratorSpec * spec = nullptr;
    std::string targetID;
    Timer intervalTimer;
    int remainingGenerations = -1;
    bool autoGenerate = true;
    bool readyToGenerate = true;
};

class EntityGeneratorComponent : public Component {
private:
    TransformComponent * transform = nullptr;
    SoundComponent * sound = nullptr;
    inline static std::unordered_map<std::string, GeneratorSpec> specs;
    inline static std::unordered_map<std::string, EntitySpawnFactory> entityFactories;

public:
    std::vector<GeneratorSlot> generatorSlots;

    EntityGeneratorComponent() = default;
    EntityGeneratorComponent(const std::string& specID, Vector2D offset = Vector2D(), float angleOffset = 0.0f,
        int maxGenerations = -2, int autoGenerateOverride = -1) {
        addGenerator(specID, offset, angleOffset, maxGenerations, autoGenerateOverride);
    }

    static GeneratorSpec * getSpec(const std::string& id) {
        auto it = specs.find(id);
        if (it != specs.end()) {
            return &it->second;
        }
        return nullptr;
    }

    static void addSpec(const std::string& id, const std::string& targetID, Uint32 interval,
        int maxGenerations = -1, bool autoGenerate = true, bool attachToEntity = false,
        const std::vector<std::pair<std::string, std::string>>& soundFiles = {},
        EntitySpawnFactory customFactory = nullptr) {
        GeneratorSpec newSpec;
        newSpec.targetID = targetID;
        newSpec.interval = interval;
        newSpec.maxGenerations = maxGenerations;
        newSpec.autoGenerate = autoGenerate;
        newSpec.attachToEntity = attachToEntity;
        newSpec.customFactory = std::move(customFactory);
        for (const auto& pair : soundFiles) {
            newSpec.sound[pair.first] = ResourceManager::loadSound(pair.second);
        }
        specs[id] = newSpec;
    }

    static void registerEntityFactory(const std::string& targetID, EntitySpawnFactory factory) {
        entityFactories[targetID] = std::move(factory);
    }

    static void clearSpecs() {
        specs.clear();
        entityFactories.clear();
    }

    void addGenerator(const std::string& specID, Vector2D offset = Vector2D(), float angleOffset = 0.0f,
        int maxGenerations = -2, int autoGenerateOverride = -1) {
        const GeneratorSpec * spec = getSpec(specID);
        if (!spec)
            return;

        GeneratorSlot slot;
        slot.specID = specID;
        slot.offset = offset;
        slot.angleOffset = angleOffset;
        slot.spec = spec;
        slot.targetID = spec->targetID;
        slot.intervalTimer = Timer(spec->interval, false);
        slot.remainingGenerations = (maxGenerations == -2) ? spec->maxGenerations : maxGenerations;
        slot.autoGenerate = (autoGenerateOverride < 0) ? spec->autoGenerate : (autoGenerateOverride != 0);
        slot.readyToGenerate = (slot.remainingGenerations != 0);

        if (sound && !spec->sound.empty()) {
            sound->addSounds(spec->sound, specID + "_");
        }
        generatorSlots.push_back(slot);
    }

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
        bool hasAnySound = false;
        for (const auto& slot : generatorSlots) {
            if (slot.spec && !slot.spec->sound.empty()) {
                hasAnySound = true;
                break;
            }
        }
        if (hasAnySound || entity->hasComponent<SoundComponent>()) {
            if (!entity->hasComponent<SoundComponent>())
                sound = &entity->addComponent<SoundComponent>();
            else
                sound = &entity->getComponent<SoundComponent>();

            for (const auto& slot : generatorSlots) {
                if (slot.spec && !slot.spec->sound.empty())
                    sound->addSounds(slot.spec->sound, slot.specID + "_");
            }
        }
    }

    void update() override {
        for (std::size_t i = 0; i < generatorSlots.size(); i++) {
            auto& slot = generatorSlots[i];
            if (!slot.spec)
                continue;

            if (slot.intervalTimer.isRunning() || slot.remainingGenerations == 0)
                slot.readyToGenerate = false;
            else
                slot.readyToGenerate = true;

            if (slot.autoGenerate && slot.readyToGenerate) {
                generate(static_cast<int>(i));
            }
        }
    }

    Entity * generate(int slotIndex, bool ignoreTimer = false) {
        if (slotIndex < 0 || slotIndex >= static_cast<int>(generatorSlots.size()))
            return nullptr;
        GeneratorSlot& slot = generatorSlots[slotIndex];
        if (!slot.spec || slot.remainingGenerations == 0)
            return nullptr;
        if (!slot.readyToGenerate && !ignoreTimer)
            return nullptr;

        Entity * spawned = nullptr;
        const std::string& targetID = slot.targetID.empty() ? slot.spec->targetID : slot.targetID;
        if (slot.spec->customFactory) {
            spawned = slot.spec->customFactory(entity, slot.offset, slot.angleOffset, slot.spec->attachToEntity);
        } else {
            auto factoryIt = entityFactories.find(targetID);
            if (factoryIt != entityFactories.end() && factoryIt->second) {
                spawned = factoryIt->second(entity, slot.offset, slot.angleOffset, slot.spec->attachToEntity);
            } else if (ParticleComponent::getSpec(targetID)) {
                spawned = ParticleComponent::spawn(
                    targetID, slot.offset, slot.angleOffset, entity, slot.spec->attachToEntity ? 1 : 0);
            }
        }

        if (sound && !slot.spec->sound.empty()) {
            sound->playSound(slot.specID + "_generate");
        }

        if (slot.remainingGenerations > 0) {
            slot.remainingGenerations--;
        }
        slot.intervalTimer.reset();
        slot.readyToGenerate = false;
        return spawned;
    }

    void generateAll(bool ignoreTimer = false) {
        for (int i = 0; i < static_cast<int>(generatorSlots.size()); i++) {
            generate(i, ignoreTimer);
        }
    }
};
