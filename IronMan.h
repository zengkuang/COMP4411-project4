#ifndef IRONMAN_H
#define IRONMAN_H

#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>
#include "modelerglobals.h"
#include "camera.h"
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.h>
#include <GL/glu.h>
#include <cstdio>
#include <math.h>
#include "modelerui.h"

class IronMan : public ModelerView
{
public:
	IronMan(int x, int y, int w, int h, char *label)
		: ModelerView(x, y, w, h, label) { }

	virtual void draw();
	void Fog();
	void drawParticles();
private:
	Mat4d CameraMatrix;
};
#endif /* IRON_MAN */