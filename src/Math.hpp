#pragma once

#include <cmath>

#define MAX(a,b) ((a)>(b))?(a):(b)
#define MIN(a,b) ((a)<(b))?(a):(b)

constexpr float PI = 3.14159265358979323846f;
constexpr float toRad = PI / 180.0f;
constexpr float toDeg = 180.0f / PI;

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

inline float clamp(float n, float floor, float ceil) {
	if (n < floor)
		return floor;
	else if (n > ceil)
		return ceil;
	else
		return n;
}

inline float clockLimit(float n, float floor, float ceil) {
	float range = ceil-floor;
    n = std::fmod(n-floor,range);
    if (n < 0) 
		n += range;
    return n+floor;
}

inline bool insideInterval(float n, float a, float b) {
	if (a < b)
		return (n >= a && n <= b)?true:false;
	return (n >= b && n <= a)?true:false;
}

inline bool angleInsideInterval(float n, float a, float b) {
    n = clockLimit(n, 0.0f, 360.0f);
    a = clockLimit(a, 0.0f, 360.0f);
    b = clockLimit(b, 0.0f, 360.0f);
    if (a <= b)
        return n >= a && n <= b;
    else
        return n >= a || n <= b;
}

inline bool aproxEqual(float n, float np) {
	return insideInterval(n,np-0.1,np+0.1);
}
