#ifndef MAYANAVDEMO_H    
#define MAYANAVDEMO_H





void pycore_vector(void);

void software_render(void);

static void display_loop();

static void animateTextures3(Image *loaded_texture);

static void ReSizeGLScene(int Width, int Height);
static void keyPressed(unsigned char key, int x, int y); 

void olmec_mouse_button(int button, int state, int x, int y);
void olmec_mouse_motion(int x, int y);

void olmec(int *argc, char** argv);





#endif

