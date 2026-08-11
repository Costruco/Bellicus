#pragma once

#include <numbers>
#include <algorithm>
#include <cmath>

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

constexpr float PI = std::numbers::pi_v<float>;
constexpr float E  = std::numbers::e_v<float>;
constexpr float toRad = PI / 180.0f;
constexpr float toDeg = 180.0f / PI;

inline float cosd(float angle) {
	return std::cos(angle * toRad);
}

inline float sind(float angle) {
	return std::sin(angle * toRad);
}

inline float tand(float angle) {
	return std::tan(angle * toRad);
}

inline float atand(float y, float x) {
	if (x == 0.0f || y == 0.0f)
		return 0.0f;
	return std::atan(y/x) * toDeg;
}

inline float sign(float n) {
	if (n > 0.0f)
		return 1.0f;
	if (n < 0.0f)
		return -1.0f;
	return 0.0f;
}

inline void toZero(float& n, float inc) {
	if (n < 0.0f)
		n = std::min(0.0f, n + inc);
	else if (n > 0.0f)
		n = std::max(0.0f, n - inc);
}

inline float moveToward(float n, float target, float inc) {
	if (n < target)
		return std::min(n + inc, target);
	if (n > target)
		return std::max(n - inc, target);
	return n;
}

inline float clamp(float n, float floor, float ceil) {
	return std::clamp(n, floor, ceil);
}

inline float clockLimit(float n, float floor, float ceil) {
	float range = ceil - floor;
	n = std::fmod(n - floor, range);
	if (n < 0.0f)
		n += range;
	return n + floor;
}

inline bool insideInterval(float n, float a, float b) {
	if (a < b)
		return n >= a && n <= b;
	return n >= b && n <= a;
}

inline bool angleInsideInterval(float n, float a, float b) {
	n = clockLimit(n, 0.0f, 360.0f);
	a = clockLimit(a, 0.0f, 360.0f);
	b = clockLimit(b, 0.0f, 360.0f);
	if (a <= b)
		return n >= a && n <= b;
	return n >= a || n <= b;
}

inline bool aproxEqual(float n, float np) {
	return insideInterval(n, np - 0.1f, np + 0.1f);
}

inline float rpmToRad(float rpm) {
	return rpm * 2.0f * PI / 60.0f;
}

inline float radToRpm(float rad) {
	return rad * 60.0f / (2.0f * PI);
}
