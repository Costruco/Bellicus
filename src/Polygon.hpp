#pragma once

#include <vector>
#include "Vector2D.hpp"
#include "Math.hpp"

class Polygon {
	public:
		std::vector<Vector2D> points;
		
		Polygon() : points{{0,0},{0,1},{1,1},{1,0}} {}
		Polygon(const std::vector<Vector2D>& pts) : points(std::move(pts)) {}
		Polygon(std::initializer_list<Vector2D> pts) : points(pts) {}
		
		Polygon rotate(const Vector2D& center, float angle) {
			std::vector<Vector2D> result;
			result.reserve(points.size());
			for (const auto& p : points) {
				result.push_back(p.rotate(center,angle));
			}
			return Polygon(result);
		}
		
		Polygon translate(const Vector2D& v) {
			std::vector<Vector2D> result;
			result.reserve(points.size());
			for (const auto& p : points) {
				result.push_back(p+v);
			}
			return Polygon(result);
		}
		
		Polygon scale(float sc) {
			std::vector<Vector2D> result;
			result.reserve(points.size());
			for (const auto& p : points) {
				result.push_back(p*sc);
			}
			return Polygon(result);
		}
};
