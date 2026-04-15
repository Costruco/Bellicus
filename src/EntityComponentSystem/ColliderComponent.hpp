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
		std::string tag;
		TransformComponent * transform;
		
		ColliderComponent(std::string tag, std::initializer_list<Vector2D> pts) : collider(pts) {
			this->tag = tag;
		}
		ColliderComponent(std::string tag, const Polygon& poly) : collider(std::move(poly)) {
			this->tag = tag;
		}
		
		void init() override {
			if (!entity->hasComponent<TransformComponent>())
				entity->addComponent<TransformComponent>();
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
		}
		
		Polygon getWorldPoints() const {
			float realScale = transform->scale*colliderScale;
			
			Vector2D center = transform->position-transform->center_offset*realScale;
			
			std::vector<Vector2D> result;
			result.reserve(collider.points.size());
			for (const auto& p : collider.points) {
				result.push_back((p+center).rotate(center,transform->direction));
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
