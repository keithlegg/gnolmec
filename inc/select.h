#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

#include "gl_setup.h"

/*
void drawTriangle (GLfloat x1, GLfloat y1, GLfloat x2, 
    GLfloat y2, GLfloat x3, GLfloat y3, GLfloat z)
{
   glBegin (GL_TRIANGLES);
   glVertex3f (x1, y1, z);
   glVertex3f (x2, y2, z);
   glVertex3f (x3, y3, z);
   glEnd ();
}
*/


#define BUFSIZE 512

void drawScene (void);


void processHits (GLint hits, GLuint buffer[]);



void selectObjects(void);

void init (void) ;

void display(void);


int selectmain(int argc, char** argv);
