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
		
		float width;
		float height;
		float scale;
		Vector2D center_offset;
			  
		TransformComponent() :
			position(),center_offset() {
			direction = 0;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = height = 0;
			scale = 1.0f;
		}
		
		TransformComponent(float xpos, float ypos, float angle, float w, float h) :
			position(xpos,ypos),center_offset() {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = 1.0f;
		}
		
		TransformComponent(float xpos, float ypos, float angle, float w, float h, float sc) :
			position(xpos,ypos),center_offset() {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = sc;
		}
				
		TransformComponent(float xpos, float ypos, float angle, float w, float h, float sc, float offx, float offy) :
			position(xpos,ypos),center_offset(offx,offy) {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = sc;
		}
		
		TransformComponent(float xpos, float ypos, float angle, float w, float h, float offx, float offy) :
			position(xpos,ypos),center_offset(offx,offy) {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
			width = w;
			height = h;
			scale = 1.0f;
		}
};
