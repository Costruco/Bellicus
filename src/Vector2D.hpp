#pragma once

#include <iostream>

class Vector2D {
	public:
		float x,
			  y;
			  
		Vector2D();
		Vector2D(float x, float y);
		
		static Vector2D fromPolar(float module, float angle);
		
		float getModule();
		float getDistance(const Vector2D& v);
		Vector2D getDirection();
		void normalize();
		
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
		
		friend std::ostream& operator<<(std::ostream& stream, const Vector2D& v);
};
