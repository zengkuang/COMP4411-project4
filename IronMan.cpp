#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "MetaBall.h"
#include <FL/gl.h>

#include "modelerglobals.h"
#include "IronMan.h"

static GLfloat lightPosition0[] = { 4, 2, -4, 0 };
static GLfloat lightDiffuse0[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat lightPosition1[] = { -2, 1, 5, 0 };
static GLfloat lightDiffuse1[] = { 1.0, 1.0, 1.0, 1.0 };

enum SampleModelControls
{
	XPOS,
	YPOS,
	ZPOS,
	HEIGHT,
	ROTATE,
	DETIAL,
	LIGHT_X,
	LIGHT_Y,
	LIGHT_Z,
	LIGHT_DENSE,
	FOG,
	ROTATE_LEFT_ARM_X,
	ROTATE_RIGHT_ARM_X,
	ROTATE_HEAD_Y,
	LIFT_RIGHT_LEG,
	LIFT_LEFT_LEG,
	PARTICLE_NUM,
	NUMCONTROLS
};
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))

ModelerView* createIronMan(int x, int y, int w, int h, char *label)
{
	return new IronMan(x, y, w, h, label);
}

static GLint fogmode;

void IronMan::Fog()
{
	glEnable(GL_FOG);
	{
		GLfloat fogColor[4] = { 0.5,0.5,0.5,1.0 };

		fogmode = GL_EXP;
		glFogi(GL_FOG_MODE, fogmode);
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, VAL(FOG));
		glHint(GL_FOG_HINT, GL_DONT_CARE);
		glFogf(GL_FOG_START, 1.0);
		glFogf(GL_FOG_END, 5.0);
	}
	glClearColor(0.5, 0.5, 0.5, 1.0);
}

void IronMan::drawParticles()
{
	Mat4d WorldMatrix = CameraMatrix.inverse() * getModelViewMatrix();
	Vec4d pos = WorldMatrix * Vec4d(0, 0, 0.2, 1);
	ParticleSystem *ps = ModelerApplication::Instance()->GetParticleSystem();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	ps->SpawnParticles(Vec3f((float)pos[0], (float)pos[1], (float)pos[2]), (int)VAL(PARTICLE_NUM), m_camera);
	glPopAttrib();
}

void IronMan::draw()
{
	ModelerView::draw();
	CameraMatrix = getModelViewMatrix();
	if (!valid())
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_NORMALIZE);
	}
	Fog();

	lightPosition0[0] = VAL(LIGHT_X);
	lightPosition0[1] = VAL(LIGHT_Y);
	lightPosition0[2] = VAL(LIGHT_Z);
	lightDiffuse0[0] = VAL(LIGHT_DENSE);
	lightDiffuse0[1] = VAL(LIGHT_DENSE);
	lightDiffuse0[2] = VAL(LIGHT_DENSE);
	lightDiffuse0[3] = VAL(LIGHT_DENSE);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse0);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse1);

	//  IRON MAN

	glPushMatrix();
	glRotated(180, 0.0, 1.0, 0.0);

	glPushMatrix();
	glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));

	// draw WING!
	drawWing();
	glPushMatrix();
	glTranslated(-1.2, 2.5, -2);
	drawTorso(2, 3, 3);
	glPopMatrix();

	//draw head
	setDiffuseColor(COLOR_GOLDEN);
	glPushMatrix();
	glTranslated(-0.2 + 0.5, 5.5, -1 + 0.5);
	glRotated(VAL(ROTATE_HEAD_Y), 0.0, 1.0, 0.0);
	glTranslated(-0.5, 0, -0.5);
	drawRectangularPrism(1.5, 1, 1);
	glPopMatrix();

	//draw left shoulder
	glPushMatrix();
	glTranslated(1.8, 4.7 + 0.75 / 2, -1 + 0.75 / 2);
	glRotated(VAL(ROTATE_LEFT_ARM_X), 1.0, 0.0, 0.0);
	glTranslated(-1.8, -4.7 - 0.75 / 2, 1 - 0.75 / 2);
	glPushMatrix();
	glTranslated(1.8, 4.7, -1);
	drawShoulder(0.75, 0.15, 2);
	glPopMatrix();

	//draw left arm
	setDiffuseColor(COLOR_RED);
	glPushMatrix();
	glTranslated(1.8, 2.2, -1);
	drawBox(0.75, 2.4, 0.75);
	glPopMatrix();

	//draw  left hand
	setDiffuseColor(COLOR_GOLDEN);
	glPushMatrix();
	glTranslated(2.2, 1.7, -0.6);
	drawSphere(0.4);
	drawParticles();
	glPopMatrix();
	glPopMatrix();


	//draw Right shoulder
	glPushMatrix();
	glTranslated(-1.2, 4.7 + 0.75 / 2, -0.3 - 0.75 / 2);
	glRotated(VAL(ROTATE_RIGHT_ARM_X), 1.0, 0.0, 0.0);
	glTranslated(1.2, -4.7 - 0.75 / 2, 0.3 + 0.75 / 2);
	setDiffuseColor(COLOR_GOLDEN);
	glPushMatrix();
	glTranslated(-1.2, 4.7, -0.3);
	glRotated(180, 0.0, 1.0, 0.0);
	drawShoulder(0.75, 0.15, 2);
	glPopMatrix();

	//draw right arm
	setDiffuseColor(COLOR_RED);
	glPushMatrix();
	glTranslated(-1.95, 2.2, -1.05);
	drawBox(0.75, 2.4, 0.75);
	glPopMatrix();

	//draw right hand
	setDiffuseColor(COLOR_GOLDEN);
	glPushMatrix();
	glTranslated(-1.6, 1.7, -0.6);
	drawSphere(0.4);
	drawParticles();
	glPopMatrix();
	glPopMatrix();


	setDiffuseColor(COLOR_RED);
	glPushMatrix();
	glTranslated(-1, 0.9 + 1.5, -1.18 + 0.5);
	glRotated(VAL(LIFT_RIGHT_LEG), -1.0, 0.0, 0.0);
	glTranslated(1, -0.9 - 1.5, 1.18 - 0.5);

	//draw right leg
	glPushMatrix();
	glTranslated(-1, 0.9, -1.18);
	drawBox(1, 1.5, 1);
	glPopMatrix();

	//draw right knee
	setDiffuseColor(COLOR_GOLDEN);
	glPushMatrix();
	glTranslated(0, 0.4, -0.7);
	glRotated(-90, 0.0, 1.0, 0.0);
	drawCylinder(1, 0.5, 0.5);
	glPopMatrix();

	//draw right lower leg
	setDiffuseColor(COLOR_RED);
	glPushMatrix();
	glTranslated(-1, -1.1 + 1, -1.18 + 0.5);
	glRotated(VAL(LIFT_RIGHT_LEG), 1.0, 0.0, 0.0);
	glTranslated(0, -1, -0.5);
	drawRectangularPrism(0.75, 1, 1);
	glTranslated(0.5, 0.0, 0.375);
	drawParticles();
	glPopMatrix();

	glPopMatrix();

	glPushMatrix();
	glTranslated(0.6, 0.9 + 1.5, -1.18 + 0.5);
	glRotated(VAL(LIFT_LEFT_LEG), -1.0, 0.0, 0.0);
	glTranslated(-0.6, -0.9 - 1.5, 1.18 - 0.5);

	//draw left leg
	setDiffuseColor(COLOR_RED);
	glPushMatrix();
	glTranslated(0.6, 0.9, -1.18);
	drawBox(1, 1.5, 1);
	glPopMatrix();

	//draw  left knee
	setDiffuseColor(COLOR_GOLDEN);
	glPushMatrix();
	glTranslated(1.6, 0.4, -0.7);
	glRotated(-90, 0.0, 1.0, 0.0);
	drawCylinder(1, 0.5, 0.5);
	glPopMatrix();

	//draw left lower leg
	setDiffuseColor(COLOR_RED);
	glPushMatrix();
	glTranslated(0.6, -1.1, -1.18);
	drawRectangularPrism(0.75, 1, 1);
	glTranslated(0.5, 0.0, 0.375);
	drawParticles();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}


int main()
{
	ModelerControl controls[NUMCONTROLS];
	controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0.0);
	controls[YPOS] = ModelerControl("Y Position", -5, 5, 0.1f, 0.0);
	controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0.0);
	controls[HEIGHT] = ModelerControl("Height", 1, 2.5, 0.1f, 1.0);
	controls[ROTATE] = ModelerControl("Rotate", -135, 135, 1, 0);
	controls[DETIAL] = ModelerControl("Number of Detial", 0, 5, 1, 0);
	controls[LIGHT_X] = ModelerControl("Light_X", -30, 30, 0.1f, 0.0);
	controls[LIGHT_Y] = ModelerControl("Light_Y", -30, 30, 0.1f, 0.0);
	controls[LIGHT_Z] = ModelerControl("Light_Z", -30, 30, 0.1f, -5.0);
	controls[LIGHT_DENSE] = ModelerControl("Light dense", 0, 5, 0.1f, 1.0);
	controls[FOG] = ModelerControl("Fog", 0, 0.25, 0.005f, 0);
	controls[ROTATE_LEFT_ARM_X] = ModelerControl("ROTATE_LEFT_ARM_X", -180, 180, 1, 0);
	controls[ROTATE_RIGHT_ARM_X] = ModelerControl("ROTATE_RIGHT_ARM_X", -180, 180, 1, 0);
	controls[ROTATE_HEAD_Y] = ModelerControl("ROTATE_HEAD_Y", -45, 45, 1, 0);
	controls[LIFT_RIGHT_LEG] = ModelerControl("LIFT_RIGHT_LEG", -45, 45, 1, 0);
	controls[LIFT_LEFT_LEG] = ModelerControl("LIFT_LEFT_LEG", -45, 45, 1, 0);
	controls[PARTICLE_NUM] = ModelerControl("Number of particles", 0, 50, 1, 20);


	ParticleSystem *ps = new ParticleSystem(3, 0.5);
	ModelerApplication::Instance()->SetParticleSystem(ps);
	ModelerApplication::Instance()->Init(&createIronMan, controls, NUMCONTROLS);
	return ModelerApplication::Instance()->Run();
}
