#ifndef GLSETUP_H    
#define GLSETUP_H


#define TRUE 1
#define FALSE 0



#include "framebuffer.h"
#include "math_op.h"

#include "obj_model.h"





struct material {
  vec3 diffuse_color;
  vec3 specular_color;
  float shininess;
  float transparency;
};


struct directional_light
{
    vec3 color;
    float ambient_intensity;
    vec3 direction;
    float diffuse_intensity;
}; 





/********************************************/
#define GL_GLEXT_PROTOTYPES

#ifdef __linux__
	#include <GL/glut.h>       // Header File For The GLUT Library 
  #include <GL/gl.h>       // Header File For The OpenGL32 Library
  //#include <GL/glu.h>      // Header File For The GLu32 Library

#endif

 
#ifdef __APPLE__
	#include <GLUT/glut.h>
  //#include <GL/gl.h>      // not in use
	//#include <GL/glew.h>    // not in use 
#endif


void InitGL(int Width, int Height);

void dump_points_GLfloat( GLfloat* verts, obj_model* pt_object, int numpts);

void set_screen_square(int* sx, int* sy);
void graticulate( bool *draw_grid, bool *draw_cntrgrid, RGBType *pt_gridcolor, RGBType *pt_gridcolor2 );
void show_bbox(bool * pt_draw_bbox, struct obj_info* pt_obinfo, RGBType *pt_gridcolor);






#endif