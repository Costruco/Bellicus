#pragma once

#include "Components.hpp"
#include "Vector2D.hpp"
#include "MovementInput.hpp"

class TransformComponent : public Component {
	public:
		Vector2D position;
		float direction;
		MovementDirection moveIntent;
		TurnDirection turnIntent;
		
		float height = 32;
		float width = 32;
		float scale = 1;
			  
		TransformComponent() :
			position() {
			direction = 0;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = height = 0;
			scale = 1;
		}
		
		TransformComponent(float xpos, float ypos, float angle) :
			position(xpos,ypos) {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = height = 0;
			scale = 1;
		}
		
		TransformComponent(float xpos, float ypos, float angle, float w, float h, float sc) :
			position(xpos,ypos) {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = sc;
		}
};
