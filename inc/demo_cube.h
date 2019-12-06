#ifndef CUBE_DEMO_H    
#define CUBE_DEMO_H



#include "framebuffer.h"; 



static void animateTextures3(Image *loaded_texture);

// static void draw_3d_cube();

static void ReSizeGLScene(int Width, int Height);
static void keyPressed(unsigned char key, int x, int y);

static void maya_mouse_motion(int x, int y);
static void maya_mouse_button(int button, int state, int x, int y);


void drawglscene_3d();
void spinningCubeDemo(int *argc, char** argv);







#endif