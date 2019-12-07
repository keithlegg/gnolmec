#ifndef GLSETUP_H    
#define GLSETUP_H


#define TRUE 1
#define FALSE 0


// typedef int bool;

#define VIEWING_DISTANCE_MIN  3.0



////////////////////////
#ifdef __linux__
#include <GL/glut.h>     // Header File For The GLUT Library 
#include <GL/gl.h>       // Header File For The OpenGL32 Library
#include <GL/glu.h>      // Header File For The GLu32 Library
#endif

////////////////////////
//OSX related 
#include <string.h>      //for memset 

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif




void InitGL(int Width, int Height);

void shader_test(void);
GLuint load_and_compile_shader(const char *fname, GLenum shaderType);



#endif