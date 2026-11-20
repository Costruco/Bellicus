#include <cmath>

#include "Camera.hpp"
#include "EntityComponentSystem/Components.hpp"
#include "Game.hpp"
#include "Map.hpp"
#include "Utils/Math.hpp"

void Camera::update() {
    offset = {Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / 2.0f};

    if (target) {
        if (!target->isActive()) {
            target = nullptr;
        } else if (target->hasComponent<TransformComponent>()) {
            const auto& transform = target->getComponent<TransformComponent>();
            focus = transform.getPosition();
            if (followAngle) {
                angle = 90.0f + transform.getDirection();
            }
        }
    }

    // Bloqueia a visao da parte exterior do mapa
    if (mapSizeX > 0 && mapSizeY > 0) {
        float left = -mapSizeX / 2 * TILE_SIZE;
        float right = (mapSizeX - mapSizeX / 2) * TILE_SIZE;
        float top = -mapSizeY / 2 * TILE_SIZE;
        float bottom = (mapSizeY - mapSizeY / 2) * TILE_SIZE;

        float cos = std::abs(cosd(angle));
        float sin = std::abs(sind(angle));
        float viewW = Game::WINDOW_WIDTH / scale.x;
        float viewH = Game::WINDOW_HEIGHT / scale.y;
        float halfSizeX = (viewW * cos + viewH * sin) * 0.5f;
        float halfSizeY = (viewW * sin + viewH * cos) * 0.5f;

        left += halfSizeX;
        right -= halfSizeX;
        top += halfSizeY;
        bottom -= halfSizeY;

        focus.x = (left > right) ? (left + right) * 0.5f : clamp(focus.x, left, right);
        focus.y = (top > bottom) ? (top + bottom) * 0.5f : clamp(focus.y, top, bottom);
    }
}
