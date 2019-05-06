#ifndef PARTICLE_H
#define PARTICLE_H

#include "vec.h"
#include <vector>
#include <map>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.h>
#include <FL/gl.h>
#include <GL/glu.h>
#include "camera.h"

class Force;

class Particle {
public:
	Particle(Vec3f p, float m, float t) : position(p), mass(m), life(t) {}

	inline void setPos(Vec3f p) { position = p; }
	inline void setSpeed(Vec3f s) { speed = s; }
	inline void setNetForce(Vec3f f) { netForce = f; }
	inline Vec3f getPos() const { return position; }
	inline Vec3f getSpeed() const { return speed; }
	inline Vec3f getNetForce() const { return netForce; }
	inline float getMass() const { return mass; }
	inline float getLife() const { return life; }

	void addForce(Force* f);
	void update(float deltaT);
	void draw(Camera* camera);

	static GLuint textureID;
private:
	float mass;
	float life;
	Vec3f position;
	Vec3f speed;
	Vec3f netForce;
	std::vector<Force*> forces;
};

#endif /*PARTICLE_H*/