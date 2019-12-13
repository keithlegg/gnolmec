#ifndef MAYANAVDEMO_H    
#define MAYANAVDEMO_H


#include "gl_setup.h"



void set_screen_square(void);

void init_pycore(void);

void software_render(void);

static void display_loop();

static void animateTextures3(Image *loaded_texture);

static void ReSizeGLScene(int Width, int Height);
static void keyPressed(unsigned char key, int x, int y); 

void glutm44_to_m44( m44* pt_m44, GLfloat m44_glfloat[16] );

void olmec_mouse_button(int button, int state, int x, int y);
void olmec_mouse_motion(int x, int y);

void olmec(int *argc, char** argv);





#endif

