#ifndef POSITION_COMPONENT_HPP
#define POSITION_COMPONENT_HPP

#include "Components.hpp"

class PositionComponent : public Component {	
	private:
		int xpos,
		    ypos;
		    
	public:
		PositionComponent() {
			xpos = ypos = 0;
		}
		PositionComponent(int x, int y) {
			xpos = x;
			ypos = y;
		}
		
		int x() {
			return xpos;
		}
		
		int y() {
			return ypos;
		}
		
		void update() override {
			xpos++;
			ypos++;
		}
		
		void setPos(int x, int y) {
			xpos = x;
			ypos = y;
		}
};

#endif
