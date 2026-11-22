#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "EntityComponentSystem.hpp"
#include "TransformComponent.hpp"
#include "ResourceManager.hpp"

class SoundComponent : public Component {
public:
    TransformComponent * transform = nullptr;
    std::unordered_map<std::string, ResourceManager::SoundPtr> sound;

    SoundComponent() = default;
    SoundComponent(const std::string& tag, const std::string& soundFile) {
        loadSound(tag, soundFile);
    }

    SoundComponent(const std::vector<std::pair<std::string, std::string>>& soundFiles) {
        for (const auto& pair : soundFiles) {
            loadSound(pair.first, pair.second);
        }
    }

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
    }

    void loadSound(const std::string& tag, const std::string& soundFile) {
        sound[tag] = ResourceManager::loadSound(soundFile);
    }

    void addSounds(const std::unordered_map<std::string, ResourceManager::SoundPtr>& newSounds, const std::string& prefix = "") {
        for (const auto& [tag, s] : newSounds) {
            sound[prefix + tag] = s;
        }
    }

    void playSound(const std::string& tag, int loops = 0, int channel = -1) {
        auto it = sound.find(tag);
        if (it != sound.end()) {
            ResourceManager::playSound(it->second, transform->getPosition(), loops, channel);
        }
    }
};