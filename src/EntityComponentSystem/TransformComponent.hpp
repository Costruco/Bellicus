#pragma once

#include "Components.hpp"
#include <vector>
#include <algorithm>
#include "Vector2D.hpp"
#include "Input.hpp"
#include "Math.hpp"

class TransformComponent : public Component {
	public:
		TransformComponent * father = nullptr;
		std::vector<TransformComponent*> sons;
		
		Vector2D position;
		Vector2D velocity;
		
		float direction;
		float angularVelocity = 0.0f;
		
		MovementDirection moveIntent;
		TurnDirection turnIntent;
		
		float width;
		float height;
		float scale;
		Vector2D center_offset;

		TransformComponent() :
			position(), velocity(), center_offset() {
			direction = 0.0f;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = 0.0f;
			height = 0.0f;
			scale = 1.0f;
		}

		TransformComponent(float xpos, float ypos, float w, float h) :
			position(xpos, ypos), velocity(), center_offset() {
			direction = 0.0f;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = 1.0f;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h) :
			position(xpos, ypos), velocity(), center_offset() {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = 1.0f;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h, float sc) :
			position(xpos, ypos), velocity(), center_offset() {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = sc;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h, float sc, float offx, float offy) :
			position(xpos, ypos), velocity(), center_offset(offx, offy) {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = sc;
		}

		TransformComponent(float xpos, float ypos, float angle, float w, float h, float offx, float offy) :
			position(xpos, ypos), velocity(), center_offset(offx, offy) {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = 1.0f;
		}
		
		void setFather(TransformComponent * newFather) {
			if (newFather == this)
				return;
		
			if (father == newFather)
				return;
		
			if (newFather && newFather->hasFather(this))
				return;
		
			if (father)
				father->removeSonOnly(this);
		
			father = newFather;
		
			if (father)
				father->addSonOnly(this);
		}

		void addSon(TransformComponent * son) {
			if (!son)
				return;
		
			son->setFather(this);
		}
		
		void removeSon(TransformComponent * son) {
			if (!son)
				return;
		
			if (son->father == this)
				son->father = nullptr;
		
			removeSonOnly(son);
		}
		
		void removeFromFather() {
			if (father)
				father->removeSonOnly(this);
		
			father = nullptr;
		}
		
		bool hasFather(TransformComponent * possibleFather) const {
			TransformComponent * current = father;
		
			while (current) {
				if (current == possibleFather)
					return true;
		
				current = current->father;
			}
		
			return false;
		}
		
		void destroySonsRecursive() {
			std::vector<TransformComponent*> sonsCopy = sons;
			sons.clear();
		
			for (TransformComponent * son : sonsCopy) {
				if (!son)
					continue;
		
				son->father = nullptr;
				son->destroySonsRecursive();
		
				if (son->entity)
					son->entity->destroy();
			}
		}
		
		Vector2D getPosition() const {
			if (father)
				return (position*father->getScale()).rotate({0, 0},father->getDirection())+father->getPosition();
			return position;
		}

		float getDirection() const {
			if (father)
				return clockLimit(direction+father->getDirection(),0.0f,360.0f);
			return direction;
		}
		
		float getScale() const {
			if (father)
				return scale*father->getScale();
			return scale;
		}
		
	private:
		void addSonOnly(TransformComponent * son) {
			if (!son)
				return;
	
			if (std::find(sons.begin(), sons.end(), son) == sons.end())
				sons.push_back(son);
		}
	
		void removeSonOnly(TransformComponent * son) {
			sons.erase(std::remove(sons.begin(), sons.end(), son), sons.end());
		}
};
