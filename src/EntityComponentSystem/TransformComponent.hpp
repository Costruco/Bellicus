#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include "Components.hpp"
#include "Vector2D.hpp"
#include "MovementInput.hpp"

class TransformComponent : public Component {
	public:
		Vector2D position;
		float direction;
		MovementDirection moveIntent;
		TurnDirection turnIntent;
			  
		TransformComponent() :
			position() {
			direction = 0;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
		}
		
		TransformComponent(float xpos, float ypos, float angle) :
			position(xpos,ypos) {
			direction = angle;
			moveIntent = MovementDirection::STILL;
			turnIntent = TurnDirection::STRAIGHT;
		}
};

#endif
