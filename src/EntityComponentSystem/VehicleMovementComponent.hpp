#ifndef VEHICLE_MOVEMENT_COMPONENT_HPP
#define VEHICLE_MOVEMENT_COMPONENT_HPP

#include "Components.hpp"
#include "Vector2D.hpp"
#include "MovementInput.hpp"

class VehicleMovementComponent : public Component {
	public:
		TransformComponent * transform;
		float speed;
			   
		VehicleMovementComponent() {
			speed = 0;
		}
		VehicleMovementComponent(float speed) {
			this->speed = speed;
		}
		
		void init() override {
			transform = &entity->getComponent<TransformComponent>();
		}
		
		void update() override {
			transform->position += Vector2D::fromPolar(speed,transform->direction);
		}	
};

#endif
