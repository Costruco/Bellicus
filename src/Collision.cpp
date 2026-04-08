#include "Collision.hpp"
#include <array>
#include "SDL2/SDL.h"
#include "Vector2D.hpp"
#include "Math.hpp"

static std::array<Vector2D,4> getCorners(const SDL_FRect& r, float angle) {
	float hw = r.w*0.5f;
    float hh = r.h*0.5f;
    
    Vector2D center{r.x+hw,r.y+hh};

    std::array<Vector2D,4> pts = {
        Vector2D{-hw,-hh},
        Vector2D{hw,-hh},
        Vector2D{hw,hh},
        Vector2D{-hw,hh}
    };

    for (auto& p : pts) {
        p = p+center;
        p = p.rotate(center,angle);
    }
    return pts;
}

static void project(const std::array<Vector2D,4>& pts, const Vector2D& axis, float& min, float& max) {
    min = max = pts[0].dot(axis);

    for (int i = 1; i < 4; ++i) {
        float p = pts[i].dot(axis);
        if (p<min) min = p;
        if (p>max) max = p;
    }
}

static bool overlap(float minA, float maxA, float minB, float maxB) {
    return !(maxA < minB || maxB < minA);
}

bool Collision::AABB(const SDL_FRect& a, const SDL_FRect& b) {
	if (a.x+a.w >= b.x && b.x+b.w >= a.x && a.y+a.h >= b.y && b.y+b.h >= a.y) {
		return true;
	}
	return false;
}

bool Collision::SAT(const SDL_FRect& a, const SDL_FRect& b, float angleA, float angleB) {
    auto A = getCorners(a,angleA);
    auto B = getCorners(b,angleB);

    for (int shape = 0; shape < 2; ++shape) {
        const auto& pts = (shape == 0) ? A : B;

        for (int i = 0; i < 4; ++i) {
            Vector2D p1 = pts[i];
            Vector2D p2 = pts[(i+1)%4];

            Vector2D edge = p2-p1;
            Vector2D axis = edge.perpendicular().normalize();

            float minA,maxA,minB,maxB;
            project(A,axis,minA,maxA);
            project(B,axis,minB,maxB);

            if (!overlap(minA,maxA,minB,maxB))
                return false;
        }
    }
    return true;
}
