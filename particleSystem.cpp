#pragma warning(disable : 4786)

#include "particleSystem.h"
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <vector>
#include <FL/gl.h>
#include "modelerdraw.h"


/***************
 * Constructors
 ***************/

ParticleSystem::ParticleSystem(float gravity, float viscosity) 
{
	srand(time(0));
	forces.push_back(new Gravity(Vec3f(0, -gravity, 0)));
	forces.push_back(new Viscous(viscosity));
}





/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem() 
{
	particles.clear();
	forces.clear();
}


/******************
 * Simulation fxns
 ******************/

/** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
	bake_start_time = t;
	bake_end_time = -1;
	simulate = true;
	dirty = true;
}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
	bake_end_time = t;
	simulate = false;
	dirty = true;
}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
	simulate = false;
	dirty = true;
}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{
	bake_fps = t - curTime;
	curTime = t;
	if (isSimulate() && !isBakedAt(t))
	{
		std::deque<Particle>::iterator it = particles.begin();
		for (std::deque<Particle>::iterator i = particles.begin(); i != particles.end(); i++)
		{
			i->update(bake_fps);
			if (i->getLife() < 0)
			{
				it = i;
			}
		}
		particles.erase(particles.begin(), it);
		bakeParticles(t);
	}
}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{
	if (isBakedAt(t))
	{
		map<float, std::deque<Particle>>::iterator it = bakeRec.find(t);
		for (std::deque<Particle>::iterator i = it->second.begin(); i != it->second.end(); i++)
		{
			i->draw(m_camera);
		}
	}
	else if (isSimulate())
	{
		for (std::deque<Particle>::iterator it = particles.begin(); it != particles.end(); it++)
		{
			it->draw(m_camera);
		}
	}
}

/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t) 
{
	bakeRec.insert(std::pair<float, std::deque<Particle>>(t, particles));
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
	bakeRec.clear();
}


bool ParticleSystem::isBakedAt(float t)
{
	map<float, std::deque<Particle>>::iterator it = bakeRec.find(t);
	return (it != bakeRec.end());
}

void ParticleSystem::SpawnParticles(Vec3f pos, int num, Camera* camera)
{
	m_camera = camera;
	if (isSimulate())
	{
		if (!isBakedAt(curTime + bake_fps))
		{
			for (int i = 0; i < num; ++i)
			{
				float mass = 1.0f + rand() / (RAND_MAX / 2.0f);
				float lifetime = 0.8f + rand() / (RAND_MAX / 0.2f);
				Particle p = Particle(pos, mass, lifetime);
				float initialSpeed = rand() / (RAND_MAX / 2.0f);
				float theta = rand() / (RAND_MAX / 360.0f);

				float xSpeed = cos(theta) * initialSpeed;
				float zSpeed = sin(theta) * initialSpeed;
				p.setSpeed(Vec3f(xSpeed, 0, zSpeed));
				for (std::vector<Force*>::iterator it = forces.begin(); it != forces.end(); it++) p.addForce(*it);
				particles.push_back(p);
			}
		}
	}
}