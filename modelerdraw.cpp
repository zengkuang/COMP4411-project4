#include "modelerdraw.h"
#include <FL/gl.h>
#include <FL/fl_ask.H>
#include <GL/glu.h>
#include <cstdio>
#include <math.h>
#include "bitmap.h"
#include "MetaBall.h"
#include "modelerglobals.h"

// ********************************************************
// Support functions from previous version of modeler
// ********************************************************
void _dump_current_modelview( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    GLdouble mv[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, mv );
    fprintf( mds->m_rayFile, 
        "transform(\n    (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n     (%f,%f,%f,%f),\n    (%f,%f,%f,%f),\n",
        mv[0], mv[4], mv[8], mv[12],
        mv[1], mv[5], mv[9], mv[13],
        mv[2], mv[6], mv[10], mv[14],
        mv[3], mv[7], mv[11], mv[15] );
}

void _dump_current_material( void )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile == NULL)
    {
        fprintf(stderr, "No .ray file opened for writing, bailing out.\n");
        exit(-1);
    }
    
    fprintf( mds->m_rayFile, 
        "material={\n    diffuse=(%f,%f,%f);\n    ambient=(%f,%f,%f);\n}\n",
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2], 
        mds->m_diffuseColor[0], mds->m_diffuseColor[1], mds->m_diffuseColor[2]);
}

// ****************************************************************************

// Initially assign singleton instance to NULL
ModelerDrawState* ModelerDrawState::m_instance = NULL;

ModelerDrawState::ModelerDrawState() : m_drawMode(NORMAL), m_quality(MEDIUM)
{
    float grey[]  = {.5f, .5f, .5f, 1};
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    
    memcpy(m_ambientColor, black, 4 * sizeof(float));
    memcpy(m_diffuseColor, grey, 4 * sizeof(float));
    memcpy(m_specularColor, white, 4 * sizeof(float));
    
    m_shininess = 0.5;
    
    m_rayFile = NULL;
}

// CLASS ModelerDrawState METHODS
ModelerDrawState* ModelerDrawState::Instance()
{
    // Return the singleton if it exists, otherwise, create it
    return (m_instance) ? (m_instance) : m_instance = new ModelerDrawState();
}

// ****************************************************************************
// Modeler functions for your use
// ****************************************************************************
// Set the current material properties

void setAmbientColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_ambientColor[0] = (GLfloat)r;
    mds->m_ambientColor[1] = (GLfloat)g;
    mds->m_ambientColor[2] = (GLfloat)b;
    mds->m_ambientColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mds->m_ambientColor);
}

void setDiffuseColor(float r, float g, float b)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_diffuseColor[0] = (GLfloat)r;
    mds->m_diffuseColor[1] = (GLfloat)g;
    mds->m_diffuseColor[2] = (GLfloat)b;
    mds->m_diffuseColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mds->m_diffuseColor);
    else
        glColor3f(r,g,b);
}

void setSpecularColor(float r, float g, float b)
{	
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_specularColor[0] = (GLfloat)r;
    mds->m_specularColor[1] = (GLfloat)g;
    mds->m_specularColor[2] = (GLfloat)b;
    mds->m_specularColor[3] = (GLfloat)1.0;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mds->m_specularColor);
}

void setShininess(float s)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    mds->m_shininess = (GLfloat)s;
    
    if (mds->m_drawMode == NORMAL)
        glMaterialf( GL_FRONT, GL_SHININESS, mds->m_shininess);
}

void setDrawMode(DrawModeSetting_t drawMode)
{
    ModelerDrawState::Instance()->m_drawMode = drawMode;
}

void setQuality(QualitySetting_t quality)
{
    ModelerDrawState::Instance()->m_quality = quality;
}

bool openRayFile(const char rayFileName[])
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	fprintf(stderr, "Ray file format output is buggy (ehsu)\n");
    
    if (!rayFileName)
        return false;
    
    if (mds->m_rayFile) 
        closeRayFile();
    
    mds->m_rayFile = fopen(rayFileName, "w");
    
    if (mds->m_rayFile != NULL) 
    {
        fprintf( mds->m_rayFile, "SBT-raytracer 1.0\n\n" );
        fprintf( mds->m_rayFile, "camera { fov=30; }\n\n" );
        fprintf( mds->m_rayFile, 
            "directional_light { direction=(-1,-1,-1); color=(0.7,0.7,0.7); }\n\n" );
        return true;
    }
    else
        return false;
}

void _setupOpenGl()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
	switch (mds->m_drawMode)
	{
	case NORMAL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_SMOOTH);
		break;
	case FLATSHADE:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glShadeModel(GL_FLAT);
		break;
	case WIREFRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glShadeModel(GL_FLAT);
	default:
		break;
	}

}

void closeRayFile()
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    
    if (mds->m_rayFile) 
        fclose(mds->m_rayFile);
    
    mds->m_rayFile = NULL;
}

void drawSphere(double r)
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, "scale(%f,%f,%f,sphere {\n", r, r, r );
        _dump_current_material();
        fprintf(mds->m_rayFile, "}))\n" );
    }
    else
    {
        int divisions; 
        GLUquadricObj* gluq;
        
        switch(mds->m_quality)
        {
        case HIGH: 
            divisions = 32; break;
        case MEDIUM: 
            divisions = 20; break;
        case LOW:
            divisions = 12; break;
        case POOR:
            divisions = 8; break;
        }
        
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluSphere(gluq, r, divisions, divisions);
        gluDeleteQuadric( gluq );
    }
}


void drawBox( double x, double y, double z )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,  
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z );
        _dump_current_material();
        fprintf(mds->m_rayFile,  "})))\n" );
    }
    else
    {
        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv( GL_MATRIX_MODE, &savemode );
        
        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glScaled( x, y, z );
        
        glBegin( GL_QUADS );
        
        glNormal3d( 0.0, 0.0, -1.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        
        glNormal3d( 0.0, -1.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 1.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        
        glNormal3d( -1.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        
        glNormal3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 0.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        
        glNormal3d( 0.0, 1.0, 0.0 );
        glVertex3d( 0.0, 1.0, 0.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        
        glNormal3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 0.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        
        glEnd();
        
        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode( savemode );
    }
}

void drawTextureBox( double x, double y, double z )
{
    // NOT IMPLEMENTED, SORRY (ehsu)
}

void drawCylinder( double h, double r1, double r2 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();
    int divisions;

	_setupOpenGl();
    
    switch(mds->m_quality)
    {
    case HIGH: 
        divisions = 32; break;
    case MEDIUM: 
        divisions = 20; break;
    case LOW:
        divisions = 12; break;
    case POOR:
        divisions = 8; break;
    }
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "cone { height=%f; bottom_radius=%f; top_radius=%f;\n", h, r1, r2 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        GLUquadricObj* gluq;
        
        /* GLU will again do the work.  draw the sides of the cylinder. */
        gluq = gluNewQuadric();
        gluQuadricDrawStyle( gluq, GLU_FILL );
        gluQuadricTexture( gluq, GL_TRUE );
        gluCylinder( gluq, r1, r2, h, divisions, divisions);
        gluDeleteQuadric( gluq );
        
        if ( r1 > 0.0 )
        {
        /* if the r1 end does not come to a point, draw a flat disk to
            cover it up. */
            
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_INSIDE );
            gluDisk( gluq, 0.0, r1, divisions, divisions);
            gluDeleteQuadric( gluq );
        }
        
        if ( r2 > 0.0 )
        {
        /* if the r2 end does not come to a point, draw a flat disk to
            cover it up. */
            
            /* save the current matrix mode. */	
            int savemode;
            glGetIntegerv( GL_MATRIX_MODE, &savemode );
            
            /* translate the origin to the other end of the cylinder. */
            glMatrixMode( GL_MODELVIEW );
            glPushMatrix();
            glTranslated( 0.0, 0.0, h );
            
            /* draw a disk centered at the new origin. */
            gluq = gluNewQuadric();
            gluQuadricDrawStyle( gluq, GLU_FILL );
            gluQuadricTexture( gluq, GL_TRUE );
            gluQuadricOrientation( gluq, GLU_OUTSIDE );
            gluDisk( gluq, 0.0, r2, divisions, divisions);
            gluDeleteQuadric( gluq );
            
            /* restore the matrix stack and mode. */
            glPopMatrix();
            glMatrixMode( savemode );
        }
    }
    
}
void drawTriangle( double x1, double y1, double z1,
                   double x2, double y2, double z2,
                   double x3, double y3, double z3 )
{
    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();

    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile, 
            "polymesh { points=((%f,%f,%f),(%f,%f,%f),(%f,%f,%f)); faces=((0,1,2));\n", x1, y1, z1, x2, y2, z2, x3, y3, z3 );
        _dump_current_material();
        fprintf(mds->m_rayFile, "})\n" );
    }
    else
    {
        double a, b, c, d, e, f;
        
        /* the normal to the triangle is the cross product of two of its edges. */
        a = x2-x1;
        b = y2-y1;
        c = z2-z1;
        
        d = x3-x1;
        e = y3-y1;
        f = z3-z1;
        
        glBegin( GL_TRIANGLES );
        glNormal3d( b*f - c*e, c*d - a*f, a*e - b*d );
        glVertex3d( x1, y1, z1 );
        glVertex3d( x2, y2, z2 );
        glVertex3d( x3, y3, z3 );
        glEnd();
    }
}
void drawRectangularPrism(double r1, double r2, double h)
{
	double a = (r2 - r1) / 2;
	double b = (r2 + r1) / 2;
	drawTriangle(0, 0, 0, r2, 0, r2, 0, 0, r2);
	drawTriangle(0, 0, 0, r2, 0, 0, r2, 0, r2);

	drawTriangle(0, 0, 0, a, h, b, a, h, a);
	drawTriangle(0, 0, 0, 0, 0, r2, a, h, b);

	drawTriangle(0, 0, 0, a, h, a, b, h, a);
	drawTriangle(0, 0, 0, b, h, a, r2, 0, 0);

	drawTriangle(r2, 0, 0, b, h, a, b, h, b);
	drawTriangle(r2, 0, 0, b, h, b, r2, 0, r2);

	drawTriangle(0, 0, r2, b, h, b, a, h, b);
	drawTriangle(0, 0, r2, r2, 0, r2, b, h, b);

	drawTriangle(a, h, a, a, h, b, b, h, b);
	drawTriangle(a, h, a, b, h, b, b, h, a);
}

void drawTorso(double r1, double r2, double h)
{
	double a = (r2 - r1) / 2;
	double b = (r2 + r1) / 2;
	setDiffuseColor(COLOR_RED);
	drawTriangle(0, 0, 0, r2, 0, r2, 0, 0, r2);
	drawTriangle(0, 0, 0, r2, 0, 0, r2, 0, r2);

	drawTriangle(0, 0, 0, a, h, b, a, h, a);
	drawTriangle(0, 0, 0, 0, 0, r2, a, h, b);

	drawTriangle(0, 0, 0, a, h, a, b, h, a);
	drawTriangle(0, 0, 0, b, h, a, r2, 0, 0);

	drawTriangle(r2, 0, 0, b, h, a, b, h, b);
	drawTriangle(r2, 0, 0, b, h, b, r2, 0, r2);


	/*
		drawTriangle(0, 0, r2, b, h, b, a, h, b);
		drawTriangle(0, 0, r2, r2, 0, r2, b, h, b);
		*/
	drawTriangle((3 * a + r2) / 4, 3 * h / 4, (3 * b + r2) / 4, a, h, b, b, h, b);
	drawTriangle(3 * b / 4, 3 * h / 4, (3 * b + r2) / 4, (3 * a + r2) / 4, 3 * h / 4, (3 * b + r2) / 4, b, h, b);
	drawTriangle((3 * a + r2) / 4, 3 * h / 4, (3 * b + r2) / 4, 0, 0, r2, a, h, b);
	drawTriangle(a*r2 / (r2 + r1) + r2 * r1 / (r2 + r1), h*r2 / (r2 + r1), b*r2 / (r2 + r1) + r2 * r1 / (r2 + r1), 0, 0, r2, (3 * a + r2) / 4, 3 * h / 4, (3 * b + r2) / 4);
	drawTriangle(r2, 0, r2, 0, 0, r2, a*r2 / (r2 + r1) + r2 * r1 / (r2 + r1), h*r2 / (r2 + r1), b*r2 / (r2 + r1) + r2 * r1 / (r2 + r1));
	drawTriangle(r2, 0, r2, a*r2 / (r2 + r1) + r2 * r1 / (r2 + r1), h*r2 / (r2 + r1), b*r2 / (r2 + r1) + r2 * r1 / (r2 + r1), 3 * b / 4, 3 * h / 4, (3 * b + r2) / 4);
	drawTriangle(r2, 0, r2, 3 * b / 4, 3 * h / 4, (3 * b + r2) / 4, b, h, b);

	drawTriangle(a, h, a, a, h, b, b, h, b);
	drawTriangle(a, h, a, b, h, b, b, h, a);
	setDiffuseColor(COLOR_WHITE);
	drawTriangle(a*r2 / (r2 + r1) + r2 * r1 / (r2 + r1), h*r2 / (r2 + r1), b*r2 / (r2 + r1) + r2 * r1 / (r2 + r1), (3 * a + r2) / 4, 3 * h / 4, (3 * b + r2) / 4, 3 * b / 4, 3 * h / 4, (3 * b + r2) / 4);


}
void drawShoulder(double r, double h, int level)
{
	glPushMatrix();

	if (level > 0)
	{
		drawBox(r, r, r);
	}
	if (level > 1)
	{
		glRotated(-90, 1.0, 0.0, 0.0);
		drawRectangularPyramid(r, h);
		glRotated(90, 1.0, 0.0, 0.0);
		glTranslated(0, r, 0);
		drawRectangularPyramid(r, h);
		glTranslated(0, 0, r);
		glRotated(90, 1.0, 0.0, 0.0);
		drawRectangularPyramid(r, h);
		glRotated(-90, 1.0, 0.0, 0.0);
		glTranslated(r, 0, 0);
		glRotated(90, 0, 1, 0);
		glRotated(90, 1, 0, 0);
		drawRectangularPyramid(r, h);
	}
	glPopMatrix();
}

void drawWing() {
	glPushMatrix();
	glTranslated(0, 4, -4);

	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_RED);

	glPushMatrix();
	glTranslated(0.5, 0.75 / 1.414 + 0.5, 0);
	glRotated(-45, 0.0, 0.0, 1.0);
	drawRectangularPrism(1.5, 0.75, 3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-0.5, 0.5, 0);
	glRotated(45, 0.0, 0.0, 1.0);
	drawRectangularPrism(1.5, 0.75, 3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.75 / 1.414 + 0.5, -0.5, 0);
	glRotated(-135, 0.0, 0.0, 1.0);
	drawRectangularPrism(1.5, 0.75, 3);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0.75 / 1.414 - 0.5, -0.55 - 0.5, 0);
	glRotated(135, 0.0, 0.0, 1.0);
	drawRectangularPrism(1.5, 0.75, 3);
	glPopMatrix();

	glPopMatrix();



}

void drawRectangularPyramid(double r, double h)
{
	drawTriangle(0, 0, 0, 0, 0, r, r, 0, r);
	drawTriangle(0, 0, 0, r, 0, r, r, 0, 0);
	drawTriangle(r / 2, h, r / 2, 0, 0, 0, 0, 0, r);
	drawTriangle(r / 2, h, r / 2, 0, 0, r, r, 0, r);
	drawTriangle(r / 2, h, r / 2, r, 0, r, r, 0, 0);
	drawTriangle(r / 2, h, r / 2, r, 0, 0, 0, 0, 0);
}

void drawTriangularPrism(double a, double b, double h, double theta)
{
	double sinT = sin(theta * M_PI / 180);
	double cosT = cos(theta * M_PI / 180);
	drawTriangle(0, 0, 0, a, 0, 0, b * cosT, 0, b * sinT);

	drawTriangle(0, 0, 0, b * cosT, 0, b * sinT, 0, h, 0);
	drawTriangle(b * cosT, 0, b * sinT, b * cosT, h, b * sinT, 0, h, 0);

	drawTriangle(b * cosT, 0, b * sinT, a, 0, 0, b * cosT, h, b * sinT);
	drawTriangle(a, 0, 0, a, h, 0, b * cosT, h, b * sinT);

	drawTriangle(0, 0, 0, 0, h, 0, a, 0, 0);
	drawTriangle(a, 0, 0, 0, h, 0, a, h, 0);

	drawTriangle(0, h, 0, b * cosT, h, b * sinT, a, h, 0);
}

void drawLsystem(int type, int size) {
	if (size == 1)
	{
		glPushMatrix();
		glBegin(GL_LINES);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 2, 0);
		glEnd();
		glTranslated(0, 2, 0);
		glRotated(-30, 0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 2, 0);
		glEnd();
		glRotated(60, 0.0, 0.0, 1.0);
		glBegin(GL_LINES);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 2, 0);
		glEnd();
		glRotated(-30, 0.0, 0.0, 1.0);
		glPopMatrix();
	}
	else {
		glPushMatrix();
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 2, 0);
		glEnd();
		glTranslated(0, 2, 0);
		glRotated(-30, 0.0, 0.0, 1.0);
		drawLsystem(type, size - 1);
		glRotated(60, 0.0, 0.0, 1.0);
		drawLsystem(type, size - 1);
		glRotated(-30, 0.0, 0.0, 1.0);
		glPopMatrix();
	}
}

void _dump_current_modelview(void);
void _dump_current_material(void);
void _setupOpenGl();
GLuint textID;
int	width, height;
unsigned char*	m_Bitmap;

int loadimage()
{
	unsigned char* imagedata;
	char* imagename = "ferry.bmp";
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if ((imagedata = readBMP(imagename, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	m_Bitmap = imagedata;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &textID);
	glBindTexture(GL_TEXTURE_2D, textID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_Bitmap);

	return 1;
}


void drawTexture()
{
	_setupOpenGl();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, textID);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-2.0, -1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-2.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(0.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(0.0, -1.0, 0.0);

	glEnd();
	glFlush();
	glDisable(GL_TEXTURE_2D);
}

void drawLight()
{
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_AQUA);
	MetaBall* m_metaBall = new MetaBall(1.0f);
	m_metaBall->addBallAbs(Vec3f(3.0f, 5.5f, 3.75f), 2.5f);
	m_metaBall->draw(10);
	delete m_metaBall;
}

void drawMetaBallBody(int ang)
{
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_RED);
	MetaBall* m_metaBall = new MetaBall(1.0f);
	m_metaBall->addBallAbs(Vec3f(-1.0f, -2.5f, 0.0f), 3.0f);
	m_metaBall->addBallAbs(Vec3f(1.0f, -2.5f, 0.0f), 3.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.0f, 0.0f), 3.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -1.0f, 0.0f), 3.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, 0.0f, 0.0f), 4.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, 1.0f, 0.0f), 3.0f);
	m_metaBall->addBallAbs(Vec3f(-1.25f, 1.0f, 0.0f), 2.0f);
	m_metaBall->addBallAbs(Vec3f(1.25f, 1.0f, 0.0f), 2.0f);
	m_metaBall->addBallAbs(Vec3f(-2.25f, 1.0f, 0.0f), 3.0f);
	m_metaBall->addBallAbs(Vec3f(2.25f, 1.0f, 0.0f), 3.0f);
	// Head
	m_metaBall->addBallAbs(Vec3f(0.0f, 1.0f + 2.0f*cos(ang*M_PI / 180), 0.0f + 2.0f*sin(ang*M_PI / 180)), 6.0f);
	m_metaBall->draw(10);
	delete m_metaBall;
}

void drawMetaBallLeftArm(int ang, int angz)
{
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_RED);
	MetaBall* m_metaBall = new MetaBall(1.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, 0.0f, 0.0f), 3.0f);
	m_metaBall->addBallAbs(Vec3f(-0.25f, -0.75f, 0.0f), 1.0f);
	m_metaBall->addBallAbs(Vec3f(-0.4f, -1.2f, 0.0f), 1.0f);
	m_metaBall->addBallAbs(Vec3f(-0.7f, -1.95f, 0.0f), 1.5f);

	m_metaBall->addBallAbs(Vec3f(-0.7f + 0.6f*sin(angz*M_PI / 180), -1.95f - 0.6f*cos(ang*M_PI / 180)*cos(angz*M_PI / 180), 0.0f + 0.6f*sin(ang*M_PI / 180)*cos(angz*M_PI / 180)), 1.0f);
	m_metaBall->addBallAbs(Vec3f(-0.7f + 1.1f*sin(angz*M_PI / 180), -1.95f - 1.1f*cos(ang*M_PI / 180)*cos(angz*M_PI / 180), 0.0f + 1.1f*sin(ang*M_PI / 180)*cos(angz*M_PI / 180)), 1.0f);
	m_metaBall->addBallAbs(Vec3f(-0.7f + 1.6f*sin(angz*M_PI / 180), -1.95f - 1.6f*cos(ang*M_PI / 180)*cos(angz*M_PI / 180), 0.0f + 1.6f*sin(ang*M_PI / 180)*cos(angz*M_PI / 180)), 1.0f);
	m_metaBall->addBallAbs(Vec3f(-0.7f + 2.6f*sin(angz*M_PI / 180), -1.95f - 2.6f*cos(ang*M_PI / 180)*cos(angz*M_PI / 180), 0.0f + 2.6f*sin(ang*M_PI / 180)*cos(angz*M_PI / 180)), 2.5f);
	m_metaBall->draw(10);
	delete m_metaBall;
}

void drawMetaBallRightArm(int ang, int angz)
{
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_RED);
	MetaBall* m_metaBall = new MetaBall(1.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, 0.0f, 0.0f), 3.0f);
	m_metaBall->addBallAbs(Vec3f(0.25f, -0.75f, 0.0f), 1.0f);
	m_metaBall->addBallAbs(Vec3f(0.4f, -1.2f, 0.0f), 1.0f);
	m_metaBall->addBallAbs(Vec3f(0.7f, -1.95f, 0.0f), 1.5f);

	m_metaBall->addBallAbs(Vec3f(0.7f - 0.6f*sin(angz*M_PI / 180), -1.95f - 0.6f*cos(ang * M_PI / 180)*cos(angz*M_PI / 180), 0.0f + 0.6f*sin(ang * M_PI / 180)*cos(angz*M_PI / 180)), 1.0f);
	m_metaBall->addBallAbs(Vec3f(0.7f - 1.1f*sin(angz*M_PI / 180), -1.95f - 1.1f*cos(ang * M_PI / 180)*cos(angz*M_PI / 180), 0.0f + 1.1f*sin(ang * M_PI / 180)*cos(angz*M_PI / 180)), 1.0f);
	m_metaBall->addBallAbs(Vec3f(0.7f - 1.6f*sin(angz*M_PI / 180), -1.95f - 1.6f*cos(ang * M_PI / 180)*cos(angz*M_PI / 180), 0.0f + 1.6f*sin(ang * M_PI / 180)*cos(angz*M_PI / 180)), 1.0f);
	m_metaBall->addBallAbs(Vec3f(0.7f - 2.6f*sin(angz*M_PI / 180), -1.95f - 2.6f*cos(ang * M_PI / 180)*cos(angz*M_PI / 180), 0.0f + 2.6f*sin(ang * M_PI / 180)*cos(angz*M_PI / 180)), 2.5f);
	m_metaBall->draw(10);
	delete m_metaBall;
}

void drawMetaBallLeftLeg(int ang)
{
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_RED);
	MetaBall* m_metaBall = new MetaBall(1.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, 0.0f, 0.0f), 3.5f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -1.0f, 0.0f), 2.5f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -1.9f, 0.0f), 2.5f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f, 0.0f), 2.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 0.6f*cos(ang*M_PI / 180), 0.0f + 0.6f*sin(ang*M_PI / 180)), 1.2f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 1.0f*cos(ang*M_PI / 180), 0.0f + 1.0f*sin(ang*M_PI / 180)), 1.5f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 1.5f*cos(ang*M_PI / 180), 0.0f + 1.5f*sin(ang*M_PI / 180)), 1.25f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 2.2f*cos(ang*M_PI / 180), 0.0f + 2.2f*sin(ang*M_PI / 180)), 1.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 3.2f*cos(ang*M_PI / 180), 0.0f + 3.2f*sin(ang*M_PI / 180)), 2.5f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 3.2f*cos(ang*M_PI / 180) + 1.0f*sin(ang*M_PI / 180), 1.0f*cos(ang*M_PI / 180) + 3.2f*sin(ang*M_PI / 180)), 1.5f);
	m_metaBall->draw(10);
	delete m_metaBall;
}

void drawMetaBallRightLeg(int ang)
{
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_RED);
	MetaBall* m_metaBall = new MetaBall(1.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, 0.0f, 0.0f), 3.5f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -1.0f, 0.0f), 2.5f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -1.9f, 0.0f), 2.5f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f, 0.0f), 2.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 0.6f*cos(ang*M_PI / 180), 0.0f + 0.6f*sin(ang*M_PI / 180)), 1.2f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 1.0f*cos(ang*M_PI / 180), 0.0f + 1.0f*sin(ang*M_PI / 180)), 1.5f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 1.5f*cos(ang*M_PI / 180), 0.0f + 1.5f*sin(ang*M_PI / 180)), 1.25f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 2.2f*cos(ang*M_PI / 180), 0.0f + 2.2f*sin(ang*M_PI / 180)), 1.0f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 3.2f*cos(ang*M_PI / 180), 0.0f + 3.2f*sin(ang*M_PI / 180)), 2.5f);
	m_metaBall->addBallAbs(Vec3f(0.0f, -2.6f - 3.2f*cos(ang*M_PI / 180) + 1.0f*sin(ang*M_PI / 180), 1.0f*cos(ang*M_PI / 180) + 3.2f*sin(ang*M_PI / 180)), 1.5f);
	m_metaBall->draw(10);
	delete m_metaBall;
}

Mat4d getModelViewMatrix()
{
	GLdouble m[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, m);
	Mat4d matMV(m[0], m[1], m[2], m[3],
		m[4], m[5], m[6], m[7],
		m[8], m[9], m[10], m[11],
		m[12], m[13], m[14], m[15]);

	return matMV.transpose();
}