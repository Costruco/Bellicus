#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>

constexpr float toRad = M_PI/180.0f;

inline float cosd(float angle) {
	return std::cos(angle*toRad);
}

inline float sind(float angle) {
	return std::sin(angle*toRad);
}

inline float tand(float angle) {
	return std::tan(angle*toRad);
}

inline bool clampLimit(float& n, float floor, float ceil) {
	if (n < floor) {
		n = floor;
		return true;
	} else if (n > ceil) {
		n = ceil;
		return true;
	} else
		return false; 
}

inline bool clockLimit(float& n, float floor, float ceil) {
	if (n < floor) {
		n = std::nextafter(ceil,floor);
		return true;
	} else if (n >= ceil) {
		n = floor;
		return true;
	} else
		return false;
}

#endif
