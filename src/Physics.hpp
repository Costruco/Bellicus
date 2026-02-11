#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "Math.hpp"

#define GRAVITY 9.807

inline float acc(float speed, float mass, float enginePower, float groundDrag, float rollingDrag) {
	speed = speed/32.5;
	if (insideInterval(speed,0,enginePower/(GRAVITY*(groundDrag-rollingDrag)*mass)))
		return GRAVITY*(groundDrag-rollingDrag)*32.5;
	else if (insideInterval(speed,-enginePower/(GRAVITY*(groundDrag-rollingDrag)*mass),0))
		return -GRAVITY*(groundDrag-rollingDrag)*32.5;
	else
		return (enginePower/(mass*speed)-rollingDrag*GRAVITY)*32.5;
}

inline float desacc(float brakePower, float mass) {
	return brakePower/mass;
}

#endif
