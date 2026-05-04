#include "Vector2D.hpp"

#include "SDL2/SDL.h"
#include "Math.hpp"

Vector2D::Vector2D() {
	x = 0.0f;
	y = 0.0f;
}
Vector2D::Vector2D(int x, int y) {
	this->x = x;
	this->y = y;
}
Vector2D::Vector2D(float x, float y) {
	this->x = x;
	this->y = y;
}
Vector2D::Vector2D(const SDL_Point& p) {
	x = p.x;
	y = p.y;
}
Vector2D::Vector2D(const SDL_FPoint& p) {
	x = p.x;
	y = p.y;
}	
Vector2D Vector2D::fromPolar(float module, float angle) {
 	return Vector2D(cosd(angle)*module,sind(angle)*module);
}
Vector2D Vector2D::max(const Vector2D& v1, const Vector2D& v2) {
	return ((v1.getModule()>=v2.getModule())?v1:v2);
}
Vector2D Vector2D::min(const Vector2D& v1, const Vector2D& v2) {
	return ((v1.getModule()<=v2.getModule())?v1:v2);
}
float Vector2D::angle() const {
	return atand(this->y,this->x);
}
 
//metodos gerais
Vector2D& Vector2D::zero() {
	*this = Vector2D();
	return *this;
}
float Vector2D::getModule() const {
	return sqrt(this->x*this->x+this->y*this->y);
}
float Vector2D::getDistance(const Vector2D& v) const {
	return (*this-v).getModule();
}
float Vector2D::dot(const Vector2D& v) const {
    return x*v.x+y*v.y;
}
float Vector2D::cross(const Vector2D& v) const {
	return x*v.y-y*v.x;
}

Vector2D Vector2D::getDirection() {
	float module = this->getModule();
	if (module)
		return (*this)/module;
	else
		return Vector2D();
}
Vector2D Vector2D::normalize() {
	return this->getDirection();
}
Vector2D Vector2D::perpendicular() {
	return {-y,x};
}
Vector2D Vector2D::rotate(const Vector2D& o, float angle) const {
	Vector2D newv = {(this->x-o.x)*cosd(angle) - (this->y-o.y)*sind(angle) + o.x,
	                 (this->x-o.x)*sind(angle) + (this->y-o.y)*cosd(angle) + o.y};
	return newv;
}
float Vector2D::angle(const Vector2D& v1, const Vector2D& v2) {
	Vector2D v1v2 = v2-v1;
	return atan2(v1.y,v1.x);
}
 
//metodos membro
Vector2D& Vector2D::add(const Vector2D& v) {
	this->x += v.x;
	this->y += v.y;
	return *this;
}
Vector2D& Vector2D::sub(const Vector2D& v) {
	this->x -= v.x;
	this->y -= v.y;	
	return *this;
}
Vector2D& Vector2D::mult(const Vector2D& v) {
	this->x *= v.x;
	this->y *= v.y;
	return *this;
}
Vector2D& Vector2D::divide(const Vector2D& v) {
	this->x /= v.x;
	this->y /= v.y;
	return *this;
}
Vector2D& Vector2D::scale(float x) {
	this->x *= x;
	this->y *= x;
	return *this;
}

//operadores
Vector2D operator+(const Vector2D& v1, const Vector2D& v2) {
	return Vector2D(v1.x+v2.x,v1.y+v2.y);
}
Vector2D operator-(const Vector2D& v1, const Vector2D& v2) {
	return Vector2D(v1.x-v2.x,v1.y-v2.y);
}
Vector2D operator*(const Vector2D& v1, const Vector2D& v2) {
	return Vector2D(v1.x*v2.x,v1.y*v2.y);
}
Vector2D operator/(const Vector2D& v1, const Vector2D& v2) {
	return Vector2D(v1.x/v2.x,v1.y/v2.y);
}
Vector2D operator*(const Vector2D& v, float x) {
	return Vector2D(v.x*x,v.y*x);
}
Vector2D operator/(const Vector2D& v, float x) {
	return Vector2D(v.x/x,v.y/x);
}

//operadores de atribuição
Vector2D& Vector2D::operator+=(const Vector2D& v) {
	return this->add(v);
}
Vector2D& Vector2D::operator-=(const Vector2D& v) {
	return this->sub(v);
}
Vector2D& Vector2D::operator*=(const Vector2D& v) {
	return this->mult(v);
}
Vector2D& Vector2D::operator/=(const Vector2D& v) {
	return this->divide(v);
}
Vector2D& Vector2D::operator*=(float x) {
	return this->scale(x);
}
Vector2D& Vector2D::operator/=(float x) {
	return this->scale(1/x);
}

Vector2D::operator SDL_FPoint() const {
    return SDL_FPoint{x,y};
}

//string
std::ostream& operator<<(std::ostream& stream, const Vector2D& v) {
	stream << "\"" <<v.x << "," << v.y << "\"";
	return stream;
}
