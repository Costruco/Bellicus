#pragma once

#include "Math.hpp"
#include "Vector2D.hpp"

#include <vector>
#include <initializer_list>
#include <utility>
#include <cmath>
#include <algorithm>

constexpr float PIXELS_PER_METER = 50.0f;
constexpr float GRAVITY = 9.81f*PIXELS_PER_METER; //lembrando que ta em pixels/s²

struct TorqueCurve {
	float maxTorque;
	float minTorque;
	float peakRPM;
	float curveIncline;
	float curveWidth;

	TorqueCurve(float maxTorque, float minTorque, float peakRPM, float curveIncline, float curveWidth) {
		this->maxTorque = maxTorque;
		this->minTorque = minTorque;
		this->peakRPM = peakRPM;
		this->curveIncline = curveIncline;
		this->curveWidth = curveWidth;
	}

	float getTorque(float rpm) const {
		float aux = curveIncline*(rpm-peakRPM);
		return (maxTorque-minTorque)*std::exp(-(aux*aux)/(curveWidth*curveWidth))+minTorque;
	}
};

struct GearBox {
	std::vector<float> ratios;
	int gear;
	float finalDrive;

	GearBox(std::initializer_list<float> ratios, float finalDrive, int gear) {
		this->ratios = ratios;
		this->finalDrive = finalDrive;
		this->gear = 0;
		setGear(gear);
	}

	GearBox(std::vector<float> ratios, float finalDrive, int gear) {
		this->ratios = std::move(ratios);
		this->finalDrive = finalDrive;
		this->gear = 0;
		setGear(gear);
	}

	void setGear(int newGear) {
		if (ratios.empty()) {
			gear = 0;
			return;
		}
		gear = clamp(newGear,0,static_cast<int>(ratios.size())-1);
	}

	void shiftUp() {
		setGear(gear+1);
	}

	void shiftDown() {
		setGear(gear-1);
	}

	int maxGear() const {
		return MAX(0,static_cast<int>(ratios.size())-1);
	}

	float totalRatio() const {
		if (ratios.empty())
			return 0.0f;
		return ratios[gear]*finalDrive;
	}

	bool inNeutral() const {
		return (ratios[gear] == 0.0f);
	}
};

struct PacejkaCurve {
	float stiffness;
	float shape;
	float peakValue;
	float curvature;

	PacejkaCurve(float stiffness, float shape, float peakValue, float curvature) {
		this->stiffness = stiffness;
		this->shape = shape;
		this->peakValue = peakValue;
		this->curvature = curvature;
	}

	float evaluate(float x) const {
		float sx = stiffness*x;
		return peakValue*std::sin(shape*std::atan(sx-curvature*(sx-std::atan(sx))));
	}
};

struct WheelPhysics {
	Vector2D localPosition;
	Vector2D size;
	bool steerable;
	bool driven;
	float steerAngle;
	float omega;
	float normalLoad;
	float slipRatio;
	float slipAngle;

	WheelPhysics() {
		localPosition = Vector2D();
		size = Vector2D();
		steerable = false;
		driven = false;
		steerAngle = 0.0f;
		omega = 0.0f;
		normalLoad = 0.0f;
		slipRatio = 0.0f;
		slipAngle = 0.0f;
	}
};

struct VehicleControl {
	float throttle;
	float brake;
	float steer;

	VehicleControl() {
		throttle = 0.0f;
		brake = 0.0f;
		steer = 0.0f;
	}
};

inline float acc(float speed, float mass, float enginePower, float groundDrag, float rollingDrag) {
	speed = speed / PIXELS_PER_METER;
	if (insideInterval(speed, 0.0f, enginePower / (GRAVITY * (groundDrag - rollingDrag) * mass)))
		return GRAVITY * (groundDrag - rollingDrag) * PIXELS_PER_METER;
	if (insideInterval(speed, -enginePower / (GRAVITY * (groundDrag - rollingDrag) * mass), 0.0f))
		return -GRAVITY * (groundDrag - rollingDrag) * PIXELS_PER_METER;
	return (enginePower / (mass * speed) - rollingDrag * GRAVITY) * PIXELS_PER_METER;
}

inline float desacc(float brakePower, float mass) {
	return brakePower / mass;
}

inline float acc(float force, float mass) {
	return force / mass;
}
