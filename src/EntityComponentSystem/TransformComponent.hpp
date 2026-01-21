#ifndef TRANSFORM_COMPONENT_HPP
#define TRANSFORM_COMPONENT_HPP

#include "Components.hpp"
#include "Vector2D.hpp"

class TransformComponent : public Component {
	public:
		Vector2D position;
			  
		TransformComponent() : position() {
		
		}

		TransformComponent(int x, int y) : position(x,y) {
			
		}

		void update() override {
		
		}	
};

#endif
