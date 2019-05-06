#ifndef FORCE_H
#define FORCE_H

#include "vec.h"
#include <vector>
#include <map>

class Particle;

class Force {
public:
	virtual void addForce(Particle* particle) = 0;
};

class Gravity : public Force {
public:
	Gravity(Vec3f v) : g(v) {}
	Vec3f g;   //gravity
	virtual void addForce(Particle* particle);
};

class Viscous : public Force {
public:
	Viscous(double v) : k(v) {}
	double k;  // k of the force
	virtual void addForce(Particle* particle);
};

#endif /*FORCE_H*/