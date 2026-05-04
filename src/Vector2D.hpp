#pragma once

#include "SDL2/SDL.h"
#include <iostream>

class Vector2D {
	public:
		float x;
		float y;
			  
		Vector2D();
		Vector2D(int x, int y);
		Vector2D(float x, float y);
		Vector2D(const SDL_Point& p);
		Vector2D(const SDL_FPoint& p);
		
		static Vector2D fromPolar(float module, float angle);
		static Vector2D max(const Vector2D& v1, const Vector2D& v2);
		static Vector2D min(const Vector2D& v1, const Vector2D& v2);
		float angle() const;
		
		Vector2D& zero();
		float getModule() const;
		float getDistance(const Vector2D& v) const;
		float dot(const Vector2D& v) const;
		float cross(const Vector2D& v) const;
		Vector2D getDirection();
		Vector2D normalize();
		Vector2D perpendicular();
		Vector2D rotate(const Vector2D& o, float angle) const;
		static float angle(const Vector2D& v1, const Vector2D& v2);
		
		Vector2D& add(const Vector2D& v);
		Vector2D& sub(const Vector2D& v);
		Vector2D& mult(const Vector2D& v);
		Vector2D& divide(const Vector2D& v);
		Vector2D& scale(float x);
		
		friend Vector2D operator+(const Vector2D& v1, const Vector2D& v2);
		friend Vector2D operator-(const Vector2D& v1, const Vector2D& v2);
		friend Vector2D operator*(const Vector2D& v1, const Vector2D& v2);
		friend Vector2D operator/(const Vector2D& v1, const Vector2D& v2);
		friend Vector2D operator*(const Vector2D& v1, float x);
		friend Vector2D operator/(const Vector2D& v1, float x);
		
		Vector2D& operator+=(const Vector2D& v);
		Vector2D& operator-=(const Vector2D& v);
		Vector2D& operator*=(const Vector2D& v);
		Vector2D& operator/=(const Vector2D& v);
		Vector2D& operator*=(float x);
		Vector2D& operator/=(float x);
		
		operator SDL_FPoint() const;
    	
		friend std::ostream& operator<<(std::ostream& stream, const Vector2D& v);
};
