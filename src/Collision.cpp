#include "Collision.hpp"
#include <array>
#include "SDL2/SDL.h"
#include "Vector2D.hpp"
#include "Polygon.hpp"
#include "Math.hpp"

static void project(const std::vector<Vector2D>& pts, const Vector2D& axis, float& min, float& max) {
    min = max = pts[0].dot(axis);

    for (size_t i = 1; i < pts.size(); ++i) {
        float p = pts[i].dot(axis);
        if (p < min) 
			min = p;
        else if (p > max) 
			max = p;
    }
}

static bool overlap(float minA, float maxA, float minB, float maxB) {
    return !(maxA < minB || maxB < minA);
}

bool Collision::SAT(Polygon a, Polygon b) {
    for (int shape = 0; shape < 2; ++shape) {
        const auto& pts = (shape == 0) ? a.points : b.points;

        for (size_t i = 0; i < pts.size(); ++i) {
            Vector2D p1 = pts[i];
            Vector2D p2 = pts[(i+1)%pts.size()];

            Vector2D edge = p2-p1;
            Vector2D axis = edge.perpendicular().normalize();

            float minA,maxA,minB,maxB;
            project(a.points,axis,minA,maxA);
            project(b.points,axis,minB,maxB);

            if (!overlap(minA,maxA,minB,maxB))
                return false;
        }
    }
    return true;
}
