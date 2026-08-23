#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include <string>
#include <vector>
#include "Vector2D.hpp"
#include "Components.hpp"
#include "Game.hpp"
#include "Polygon.hpp"

class ColliderComponent : public Component {
	public:
		Polygon collider;
		float colliderScale = 1;
		const std::string& tag;
		TransformComponent * transform;
		
		ColliderComponent() = default;
		ColliderComponent(const std::string& tag, std::initializer_list<Vector2D> pts) : collider(pts), tag(tag) {
		}
		ColliderComponent(const std::string& tag, const Polygon& poly) : collider(std::move(poly)), tag(tag) {
		}
		
		void init() override {
			if (!entity->hasComponent<TransformComponent>())
				entity->addComponent<TransformComponent>();
			transform = &entity->getComponent<TransformComponent>();
			
			Game::colliders.push_back(this);
		}
		
		void update() override {
		}
		
		Polygon getWorldPoints() const {
			float realScale = transform->getScale()*colliderScale;
			Vector2D center = transform->getPosition();
		
			std::vector<Vector2D> result;
			result.reserve(collider.points.size());
		
			for (const auto& p : collider.points) {
				Vector2D scaledPoint = p*realScale;
				Vector2D rotatedPoint = scaledPoint.rotate({0, 0},transform->getDirection());
				result.push_back(center+rotatedPoint);
			}
		
			return Polygon(result);
		}
		
		void draw() override {
			Polygon trueCollider = this->getWorldPoints();
			size_t size = trueCollider.points.size();
			for (size_t i = 0; i < size; i++) {
				lineRGBA(Game::ren,trueCollider.points[i].x,trueCollider.points[i].y,
				                   trueCollider.points[(i+1)%size].x,trueCollider.points[(i+1)%size].y,
								   255,0,0,255);
			}
		}
};
