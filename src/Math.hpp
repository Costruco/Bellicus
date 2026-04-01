#pragma once

#include <cmath>

#define MAX(a,b) (a>b)?a:b
#define MIN(a,b) (a<b)?a:b

constexpr float toRad = M_PI/180.0f;
constexpr float toDeg = 180.0f/M_PI;

inline float cosd(float angle) {
	return std::cos(angle*toRad);
}

inline float sind(float angle) {
	return std::sin(angle*toRad);
}

inline float tand(float angle) {
	return std::tan(angle*toRad);
}

inline void toZero(float& n, float inc) {
	if (n < 0) 
		n = MIN(0,n+inc);
	else if (n > 0)
		n = MAX(0,n-inc);
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

inline bool insideInterval(float n, float a, float b) {
	if (a < b)
		return (n >= a && n <= b)?true:false;
	return (n >= b && n <= a)?true:false;
}

inline bool angleInsideIntervalo(float n, float a, float b) {
	if (b >= 360 && n <= 180) {
		return insideInterval(n,a-360,clockLimit(b,0,360));
	}
	else if (a <= 0 && n >= 180) {
		return insideInterval(n-360,a,b);
	}
	return insideInterval(n,a,b);
}

inline bool aproxEqual(float n, float np) {
	return insideInterval(n,np-0.1,np+0.1);
}
