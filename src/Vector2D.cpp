#include "Vector2D.hpp"

#include "Math.hpp"

Vector2D::Vector2D() {
	x = 0.0f;
	y = 0.0f;
}
Vector2D::Vector2D(float x, float y) {
	this->x = x;
	this->y = y;
}
Vector2D Vector2D::fromPolar(float module, float angle) {
 	return Vector2D(cosd(angle)*module,sind(angle)*module);
}
 
//metodos gerais
float Vector2D::getModule() {
	return sqrt(this->x*this->x+this->y*this->y);
}
float Vector2D::getDistance(const Vector2D& v) {
	return (*this-v).getModule();
}
Vector2D Vector2D::getDirection() {
	float module = this->getModule();
	if (module)
		return (*this)/module;
	else
		return Vector2D();
}
void Vector2D::normalize() {
	*this = this->getDirection();
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

//string
std::ostream& operator<<(std::ostream& stream, const Vector2D& v) {
	stream << "\"" <<v.x << "," << v.y << "\"";
	return stream;
}
