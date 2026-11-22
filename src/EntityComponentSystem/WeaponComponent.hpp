#pragma once

#include <algorithm>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <vector>

#include "Game.hpp"
#include "Components.hpp"
#include "Utils/Math.hpp"
#include "Utils/Timer.hpp"
#include "Utils/Vector2D.hpp"

struct WeaponSpec {
    int magazineSize;
    float precisionError;
    int reloadTime;
    float shootingSpeed;
    std::string projectileID;
    std::unordered_map<std::string,ResourceManager::SoundPtr> sound;
};

struct GunSlot {
    std::string specID;
    Vector2D offset;
    float angleOffset = 0.0f;
    const WeaponSpec * spec = nullptr;
    std::string projectileID;
    Timer fireTimer;
    Timer reloadTimer;
    int currentAmmo = 0;
    int ammoReserve = 0;
    bool reloading = false;
    bool readToShoot = false;
};

class WeaponComponent : public Component {
private:
    TransformComponent * transform = nullptr;
    SoundComponent * sound = nullptr;
    inline static std::unordered_map<std::string,WeaponSpec> specs;

public:
    std::vector<GunSlot> gunSlots;

    WeaponComponent() = default;
    WeaponComponent(const std::string& specID, Vector2D offset = Vector2D(), float angleOffset = 0.0f, int ammoReserve = -1) {
        addGun(specID, offset, angleOffset, ammoReserve);
    }

    static WeaponSpec * getSpec(const std::string& id) {
        auto it = specs.find(id);
        if (it != specs.end()) {
            return &it->second;
        }
        return nullptr;
    }

    static void addSpec(const std::string& id, int magazineSize, float precisionError, int reloadTime, float shootingSpeed, 
        const std::string& projectileID, const std::vector<std::pair<std::string,std::string>>& soundFiles) {
        WeaponSpec newSpec;
        newSpec.magazineSize = magazineSize;
        newSpec.precisionError = precisionError;
        newSpec.reloadTime = reloadTime;
        newSpec.shootingSpeed = shootingSpeed;
        newSpec.projectileID = projectileID;
        for (const auto& pair : soundFiles) {
            newSpec.sound[pair.first] = ResourceManager::loadSound(pair.second);
        }
        specs[id] = newSpec;
    }

    static void clearSpecs() {
        specs.clear();
    }

    void addGun(const std::string& specID, Vector2D offset = Vector2D(), float angleOffset = 0.0f, int ammoReserve = -1) {
        const WeaponSpec * spec = getSpec(specID);
        if (!spec)
            return;

        Uint32 fireCooldown = (spec->shootingSpeed > 0.0f)
            ? static_cast<Uint32>(1000.0f / spec->shootingSpeed)
            : 0;

        GunSlot slot;
        slot.specID = specID;
        slot.offset = offset;
        slot.angleOffset = angleOffset;
        slot.spec = spec;
        slot.projectileID = spec->projectileID;
        slot.fireTimer = Timer(fireCooldown, false);
        slot.reloadTimer = Timer(static_cast<Uint32>(spec->reloadTime), false);
        slot.currentAmmo = spec->magazineSize;
        slot.ammoReserve = ammoReserve;
        slot.reloading = false;
        slot.readToShoot = true;
        if (sound)
            sound->addSounds(spec->sound,specID+"_");
        gunSlots.push_back(slot);
    }

    void init() override {
        transform = &entity->getComponent<TransformComponent>();
        if (!entity->hasComponent<SoundComponent>())
            sound = &entity->addComponent<SoundComponent>();
        else
            sound = &entity->getComponent<SoundComponent>();
        
        for (const auto& slot : gunSlots) {
            if (slot.spec)
                sound->addSounds(slot.spec->sound,slot.specID+"_");
        }
    }

    void reload(int slotIndex) {
        if (slotIndex < 0 || slotIndex >= static_cast<int>(gunSlots.size()))
            return;
        GunSlot& slot = gunSlots[slotIndex];
        if (!slot.spec)
            return;
        if (!slot.reloading && slot.currentAmmo < slot.spec->magazineSize && slot.ammoReserve != 0) {
            sound->playSound(slot.specID+"_reload");
            slot.reloading = true;
            slot.reloadTimer.reset();
        }
    }

    void update() override {
        for (std::size_t i = 0; i < gunSlots.size(); i++) {
            auto &slot = gunSlots[i];
            if (!slot.spec)
                continue;

            if (slot.currentAmmo <= 0 && !slot.reloading && slot.ammoReserve != 0) {
                reload(static_cast<int>(i));
            }

            if (slot.reloading && !slot.reloadTimer.isRunning()) {
                if (slot.ammoReserve < 0) {
                    slot.currentAmmo = slot.spec->magazineSize;
                } else {
                    int needed = slot.spec->magazineSize - slot.currentAmmo;
                    int toLoad = std::min(needed, slot.ammoReserve);
                    slot.currentAmmo += toLoad;
                    slot.ammoReserve -= toLoad;
                }
                slot.reloading = false;
            }

            if (slot.fireTimer.isRunning() || slot.currentAmmo <= 0 || slot.reloading)
                slot.readToShoot = false;
            else
                slot.readToShoot = true;
        }
    }

    void shoot(int slotIndex) {
        if (slotIndex < 0 || slotIndex >= static_cast<int>(gunSlots.size()))
            return;
        GunSlot& slot = gunSlots[slotIndex];
        if (slot.readToShoot && slot.spec) {
            const std::string& projID = slot.projectileID.empty() ? slot.spec->projectileID : slot.projectileID;
            ProjectileSpec * projSpec = ProjectileComponent::getSpec(projID);
            if (!projSpec)
                return;

            float errorAngle = 0.0f;
            if (slot.spec->precisionError > 0.0f) {
                float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
                errorAngle = (r * 2.0f - 1.0f) * slot.spec->precisionError;
            }

            float entityAngle = transform->getDirection();
            float entityScale = transform->getScale();
            float bulletAngle = clockLimit(entityAngle + slot.angleOffset + errorAngle, 0.0f, 360.0f);

            int texW = 0, texH = 0;
            if (projSpec->texture) {
                SDL_QueryTexture(projSpec->texture.get(), nullptr, nullptr, &texW, &texH);
            }

            Vector2D rotatedOffset = (slot.offset * entityScale).rotate({0, 0}, entityAngle);
            Vector2D spawnPos = transform->getPosition()+rotatedOffset+Vector2D::fromPolar(bulletAngle,texW*0.5f*entityScale);

            Entity& newBullet = manager.addEntity();
            newBullet.addComponent<TransformComponent>(spawnPos, bulletAngle, static_cast<float>(texW), static_cast<float>(texH), entityScale);
            newBullet.addComponent<ProjectileComponent>(projID);
            newBullet.addComponent<SpriteComponent>(projSpec->texture);
            newBullet.addGroup(groupProjectiles);

            sound->playSound(slot.specID+"_fire");
            slot.currentAmmo--;
            slot.fireTimer.reset();
            slot.readToShoot = false;

            if (slot.currentAmmo <= 0 && slot.ammoReserve != 0) {
                reload(slotIndex);
            }
        }
    }

    void shootAll() {
        for (int i = 0; i < static_cast<int>(gunSlots.size()); i++) {
            shoot(i);
        }
    }
};
