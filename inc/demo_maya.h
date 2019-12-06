#ifndef MAYANAVDEMO_H    
#define MAYANAVDEMO_H





static void draw_3d_cube();
static void animateTextures3(Image *loaded_texture);

static void ReSizeGLScene(int Width, int Height);
static void keyPressed(unsigned char key, int x, int y); 

void maya_mouse_button(int button, int state, int x, int y);
void maya_mouse_motion(int x, int y);
void mouse_wheel(int button, int dir, int x, int y);


//void mayanav_start (void );
//void mayanav_rst_xforms( void );
void maya_navigation_demo(int *argc, char** argv);





#endif

