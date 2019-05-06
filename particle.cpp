#include "particle.h"
#include "modelerdraw.h"
#include <FL/gl.h>
#include <FL/glut.h>
#include <GL/glu.h>
#include <cstdio>
#include <math.h>
#include "Force.h"

GLuint Particle::textureID = 0;

void Particle::addForce(Force* f) {
	forces.push_back(f);
}

void Particle::update(float dt) {
	for (std::vector<Force*>::iterator it = forces.begin(); it != forces.end(); it++)
	{
		(*it)->addForce(this);
	}
	speed += netForce / mass * dt;
	position += speed * dt;
	life -= dt;
}

void Particle::draw(Camera* camera) {
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPushMatrix();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glTranslatef(position[0], position[1], position[2]);
	Vec3f up = camera->getUpVector();
	Vec3f pos = camera->getPosition();
	Vec3f delta = pos - position;
	delta.normalize();
	Vec3f right = delta ^ up;
	up = delta ^ right;

	GLfloat matrix[16];
	matrix[0] = right[0]; matrix[4] = up[0]; matrix[8] = -delta[0]; matrix[12] = 0.0;
	matrix[1] = right[1]; matrix[5] = up[1]; matrix[9] = -delta[1]; matrix[13] = 0.0;
	matrix[2] = right[2]; matrix[6] = up[2]; matrix[10] = -delta[2]; matrix[14] = 0.0;
	matrix[3] = 0.0; matrix[7] = 0.0; matrix[11] = 0.0; matrix[15] = 1.0;
	glMultMatrixf(matrix);


	glScalef(life * 2, life * 2, life * 2);
	glBegin(GL_QUADS);
	setDiffuseColor(COLOR_FIRE);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.1f, -0.1f, 0.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.1f, 0.1f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.1f, 0.1f, 0.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.1f, -0.1f, 0.0f);
	glEnd();

	glPopMatrix();
	glPopAttrib();
}