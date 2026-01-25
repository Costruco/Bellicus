#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include "Components.hpp"
#include "Vector2D.hpp"

class TransformComponent : public Component {
	public:
		Vector2D position;
		Vector2D direction;
		float speed;
			  
		TransformComponent() : 
			position(),
			direction() {
			speed = 0;
		}

		TransformComponent(float xpos, float ypos, float xdir, float ydir, float v) : 
			position(xpos,ypos),
			direction(xdir,ydir) {
			speed = v;
		}
		
		void init() override {
			direction.normalize();
		}
		
		void update() override {
			position += direction*speed;
		}	
};

#endif
