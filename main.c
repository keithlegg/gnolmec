
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>      //for getcwd 

//to get on OSX 'ls /dev/tty.*'  
//#define SERIAL_PORT_ID "/dev/tty.usbserial-FTWWRSIBA" // "/dev/tty.usbserial-A400gnmx" // "/dev/ttyUSB0"
//#define OUTPUT_BINARY "/Users/klegg/image.bin"

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

////////////////////////


#include "point_op.h"    // vector operations
#include "framebuffer.h" // raster operations
#include "image_util.h"  // experimental features, etc 


typedef int BOOL;
#define TRUE 1
#define FALSE 0
#define VIEWING_DISTANCE_MIN  3.0

static BOOL g_bButton1Down = FALSE;

//popup menu settings
static BOOL g_bLightingEnabled = TRUE;
static BOOL g_bFillPolygons = TRUE;
static BOOL g_bTexture = FALSE;

//static GLfloat g_nearPlane = 1;
//static GLfloat g_farPlane = 1000;

static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
static int g_Width  = 600;                          // Initial window width
static int g_Height = 600;                         // Initial window height
static int g_yClick = 0;
static float g_lightPos[4] = { 10, 10, -100, 1 };  // Position of light



enum {
    MENU_ABOUT = 1,
    MENU_POLYMODE,
    MENU_TEXTURING,
    MENU_EXIT
};

void SelectFromMenu(int idCommand)
{
  switch (idCommand)
    {
        case MENU_ABOUT:
          g_bLightingEnabled = !g_bLightingEnabled;
          if (g_bLightingEnabled)
             glEnable(GL_LIGHTING);
          else
             glDisable(GL_LIGHTING);
        break;

        case MENU_POLYMODE:
          g_bFillPolygons = !g_bFillPolygons;
          glPolygonMode (GL_FRONT_AND_BACK, g_bFillPolygons ? GL_FILL : GL_LINE);
        break;      

        case MENU_TEXTURING:
          g_bTexture = !g_bTexture;
          if (g_bTexture)
             glEnable(GL_TEXTURE_2D);
          else
             glDisable(GL_TEXTURE_2D);
        break;    

        case MENU_EXIT:
          exit (0);
        break;
    }

  // Almost any menu selection requires a redraw
  glutPostRedisplay();
}


/***************************************************************/

int window; // The number of our GLUT window 
 
GLuint texture[1]; // storage for one texture  

float xrot, yrot, zrot;// floats for x rotation, y rotation, z rotation 

// data for animated "pong" texture 
unsigned int upos = 100;
unsigned int vpos = 320;  
unsigned int dir_u = 0;
unsigned int dir_v = 0;
unsigned int pong_speed = 5;
unsigned int pong_size  = 60;
unsigned int img_usize  = 512;
unsigned int img_vsize  = 512;

// int steps[8] = {1024,512,256,128,64,32,16,8};
// int step_idx = 2;
int use_tex = 0;



Image* main_bg_bfr      = createBufferImage(512,512); 
Image* imageloaded_bfr  = createBufferImage(512,512); 
Image* imageloaded_bfr2 = createBufferImage(512,512); 



/***************************************/

struct rec
{
    int x,y,z;
};


int testLoadBinary()
{
    int counter;
    FILE *ptr_myfile;
    struct rec my_record;

    ptr_myfile=fopen("test.bin","rb");
    if (!ptr_myfile)
    {
        printf("Unable to open file!");
        return 1;
    }
    for ( counter=1; counter <= 10; counter++)
    {
        fread(&my_record,sizeof(struct rec),1,ptr_myfile);
        printf("%d\n",my_record.x);
    }
    
    fclose(ptr_myfile);
    return 0;

}


/***************************************/

int ImageLoad(char *filename, Image *image) 
{
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.

    if ((file = fopen(filename, "rb"))==NULL)
    {
      printf("File Not Found : %s\n",filename);
      return 0;
    }
    
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width
    if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
      printf("Error reading width from %s.\n", filename);
      return 0;
    }
    //printf("Width of %s: %lu\n", filename, image->sizeX);
    
    // read the height 
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
      printf("Error reading height from %s.\n", filename);
      return 0;
    }
    //printf("Height of %s: %lu\n", filename, image->sizeY);
    
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {
      printf("Error reading planes from %s.\n", filename);
      return 0;
    }
    if (planes != 1) {
      printf("Planes from %s is not 1: %u\n", filename, planes);
      return 0;
    }

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
      printf("Error reading bpp from %s.\n", filename);
      return 0;
    }
    if (bpp != 24) {
      printf("Bpp from %s is not 24: %u\n", filename, bpp);
      return 0;
    }
  
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data. 
    //image->data = (char *) malloc(size);  
    if (image->data == NULL) {
      printf("Error allocating memory for color-corrected image data");
      return 0; 
    }

    if ((i = fread(image->data, size, 1, file)) != 1) {
      printf("Error reading image data from %s.\n", filename);
      return 0;
    }

    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
      temp = image->data[i];
      image->data[i] = image->data[i+2];
      image->data[i+2] = temp;
    }

    //free(image->data);
    return 1;
}
    
/***************************************/

//example to manually create a buffer and fill it with a color 
int dynamicImage(Image *image) 
{
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.
   

    image->sizeX = 256;
    image->sizeY = 256;

    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    //printf("#### image mem size is %lu \n", size );
    image->data = (char *) malloc(size);

    if (image->data == NULL) {
        printf("Error allocating memory for image data");
        return 0; 
    }
 
    // iterate data and do something 
    for (i=0;i<size;i+=3) { 
        image->data[i]    = (unsigned int)0;
        image->data[i+1]  = (unsigned int)255; //image->data[i+2];
        image->data[i+2]  = (unsigned int)255; //temp;
    }

    free(image->data);
    return 1;
}

//second example to use a builtin util to fill a buffer with color 
int dynamicImage2(Image *image) 
{
    unsigned long size; 

    image->sizeX = img_usize;
    image->sizeY = img_vsize;

    size = image->sizeX * image->sizeY * 3;

    image->data = (char *) malloc(size);  

    RGBType bgcolor;
    RGBType *pt_bgcolor = &bgcolor;

    pt_bgcolor->r = 25;
    pt_bgcolor->g = 30;
    pt_bgcolor->b = 40;


    RGBType linecolor;
    RGBType *pt_linecolor = &linecolor;
    pt_linecolor->g = 230;
    pt_linecolor->b = 130;
   
    fillbuffer24(image, pt_bgcolor);
    
    draw_point ( image, image->sizeX, 0, 0, pt_linecolor  );
    draw_point ( image, image->sizeX, 511, 511, pt_linecolor  );
    draw_point ( image, image->sizeX, 511, 0, pt_linecolor  );

    //origin is bottom left - grr 
    int tl[2] = {10,10};
    int br[2] = {50,50};

    draw_square( image, image->sizeX, tl, br, pt_linecolor );

    free(image->data);
    return 1;
}

/***************************************/


void animateTextures2(Image *loaded_texture)
{

    RGBType bgcolor;
    RGBType *pt_bgcolor = &bgcolor;
    pt_bgcolor->r = 45;
    pt_bgcolor->g = 50;
    pt_bgcolor->b = 50;

    RGBType linecolor;
    RGBType *pt_linecolor = &linecolor;
    pt_linecolor->g = 230;
    pt_linecolor->b = 130;
    
    RGBType bordercolor;
    RGBType *pt_bordercolor = &bordercolor;
    pt_bordercolor->b = 255;


    RGBType hitcolor;
    RGBType *pt_hitcolor = &hitcolor;
    pt_hitcolor->r = 200;
    pt_hitcolor->b = 128;
  
    RGBType firecolor;
    RGBType *pt_firecolor = &firecolor;
    pt_firecolor->r = 227;
    pt_firecolor->g = 171;
    pt_firecolor->b = 16;    


    int cp_tl[2] = {0};
    int cp_br[2] = {0};
    
    cp_tl[0] = upos-pong_size;
    cp_tl[1] = vpos-pong_size;
    cp_br[0] = upos+pong_size;
    cp_br[1] = vpos+pong_size;



   
    //clear background image 
    //fillbuffer24(loaded_texture, pt_bgcolor);

    // draw background image 
    if (use_tex==0){ copyBuffer24( imageloaded_bfr  , loaded_texture );}
    if (use_tex==1){ copyBuffer24( imageloaded_bfr2 , loaded_texture );}

    //draw the pong bouncer 
    if(upos>(loaded_texture->sizeX-(pong_size*2)))
    {
        draw_fill_square(  loaded_texture,  upos, vpos, pong_size*1.2, pt_hitcolor  );
        draw_fill_square(  loaded_texture,  upos, vpos, pong_size, pt_firecolor  );
    }else{
        draw_fill_square(  loaded_texture,  upos, vpos, pong_size, pt_linecolor  );
    }

    copyBuffer24( imageloaded_bfr2 , loaded_texture, cp_tl, cp_br );

    


    ////

    //origin is bottom left - grr 
    int tl[2] = { 0, 0 };
    int br[2] = { loaded_texture->sizeX-1, loaded_texture->sizeY-1 };
    draw_square( loaded_texture, loaded_texture->sizeX, tl, br, pt_bordercolor );

    ////

    // Create Texture   
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    glTexImage2D(GL_TEXTURE_2D, 0, 3, loaded_texture->sizeX, loaded_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, loaded_texture->data);
    
    //free(loaded_texture->data);


}



/***************************************/

// initialize OpenGL  
void InitGL(int Width, int Height)          // We call this right after our OpenGL window is created.
{
        
    glEnable(GL_TEXTURE_2D);             // Enable Texture Mapping
    glClearColor(0.0f, 0.0f, .1f, 0.0f); // Clear The Background Color To Blue 
    glClearDepth(1.0);                   // Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);                // The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);             // Enables Depth Testing
    glShadeModel(GL_SMOOTH);             // Enables Smooth Color Shading
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                    // Reset The Projection Matrix

    //gluOrtho2D(0, 100, 0, 100); //(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);   // Calculate The Aspect Ratio Of The Window
    
    glMatrixMode(GL_MODELVIEW);
}

/***************************************/

// callback when window is resized (which shouldn't happen in fullscreen) 
void ReSizeGLScene(int Width, int Height)
{
    
    if (Height==0)  // Prevent A Divide By Zero If The Window Is Too Small
    Height=1;

    glViewport(0, 0, Width, Height);    // Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //gluOrtho2D(0, 100, 0, 100);
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    
    glMatrixMode(GL_MODELVIEW);
}//end resize callback


/***************************************/
/*
  cobbled together display callback to create a quad polygon with a UV map 
*/
 

void DrawGLScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear The Screen And The Depth Buffer
    glLoadIdentity(); // Reset The View
    glTranslatef(0.0f, 0.0f, -3.7f);    // move 5 units into the screen.
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.

    //define geometry 
    glBegin(GL_QUADS);//draw a four sided polygon(s)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // BL texture and quad
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // BR texture and quad
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // TR texture and quad
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // TL texture and quad
    glEnd(); // end polygon.
    
    /*
    glBegin(GL_POINTS);
        glVertex2f(0.0, 0.0);
        glVertex2f(1.0, 0.0);
        glVertex2f(0.0, 1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(0.5, 0.5);
    glEnd();
    */

    /*
    glBegin(GL_LINES);
        glVertex2f(0.0, 0.0);
        glVertex2f(1.0, 0.0);
    glEnd();
    */

    unsigned int u_edge = img_usize - pong_size;
    unsigned int v_edge = img_vsize - pong_size;

    // animate some numbers 
    if(upos>=u_edge)     { dir_u = 1; }
    if(upos<=pong_size)  { dir_u = 0; }

    if(vpos>=u_edge)    { dir_v = 1; }
    if(vpos<=pong_size) { dir_v = 0; }

    if(dir_u==0)        { upos +=pong_speed;}else{ upos -=pong_speed;}
    if(dir_v==0)        { vpos +=pong_speed;}else{ vpos -=pong_speed;}

    //usleep(200000); //if you want to slow it down 

    
    animateTextures2(main_bg_bfr);

    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
}//end display callback 

/***************************************/




void DrawGLScene3D()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear The Screen And The Depth Buffer
    glLoadIdentity();               // Reset The View

    glTranslatef(0.0f, 0.0f, -4.0f);              // move 5 units into the screen.
    glRotatef(xrot,1.0f,0.0f,0.0f);     // Rotate On The X Axis
    glRotatef(yrot,0.0f,1.0f,0.0f);     // Rotate On The Y Axis
    glRotatef(zrot,0.0f,0.0f,1.0f);     // Rotate On The Z Axis
    
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.

    glBegin(GL_QUADS);                      // begin drawing a cube
    
    // Front Face (note that the texture's corners have to match the quad's corners)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
    
    // Back Face
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
    
    // Top Face
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    
    // Bottom Face       
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    
    // Right face
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    
    // Left Face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
    
    glEnd();  // done with the polygon.

    if (yrot<100){
        //xrot+=.1f;  // X Axis Rotation  
        yrot+=.2f;    // Y Axis Rotation
        //zrot+=.1f;  // Z Axis Rotation
    }
    
    if (yrot>=100 && yrot<200){
        xrot+=.2f;    // X Axis Rotation  
        yrot+=.2f;    // Y Axis Rotation
        //zrot+=.1f;  // Z Axis Rotation
    }

    if (yrot>=200){
        xrot-=.2f;    // X Axis Rotation  
        //yrot+=.2f;    // Y Axis Rotation
        zrot+=.1f;  // Z Axis Rotation
 
    }

    /**********/

    unsigned int u_edge = img_usize - pong_size;
    unsigned int v_edge = img_vsize - pong_size;

    if(upos<0)         { upos = 100; }
    if(vpos<0)         { vpos = 100; }
    if(upos>img_usize) { upos = 100; }
    if(vpos>img_vsize) { vpos = 100; }


    // animate some numbers 
    if(upos>=u_edge)     { dir_u = 1; }
    if(upos<=pong_size)  { dir_u = 0; }

    if(vpos>=u_edge)    { dir_v = 1; }
    if(vpos<=pong_size) { dir_v = 0; }

    if(dir_u==0)        { upos +=pong_speed;}else{ upos -=pong_speed;}
    if(dir_v==0)        { vpos +=pong_speed;}else{ vpos -=pong_speed;}

    animateTextures2(main_bg_bfr);

    //usleep(100000); 
    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
}


/***************************************/

//define keyboard events 
void keyPressed(unsigned char key, int x, int y) 
{

    usleep(100);

    //ESCAPE KEY
    if (key == 27) 
    { 
        glutDestroyWindow(window); 
        exit(0);                   
    }
    
    if (key == 97) //a
    { 
        if (pong_speed<20){
            pong_speed +=1;
        }

    }
 
    if (key == 98) //b
    { 
        //printf("you pressed b\n");
        if (pong_speed>0){
            pong_speed -=1;
        }

    }
  
    if (key == 99) //c
    { 
        if (pong_size<200){
            pong_size +=1;
        }      
    }

    if (key == 100) //d
    { 
 
        if (pong_size>4){
            pong_size -=1;
        }

    }

    if (key == 102) //f
    { 
        //printf("you pressed f\n");
        glutFullScreen();
    }

    if (key == 119) //w
    { 
        if(use_tex==0){
            use_tex=1;
        }
        else{
            use_tex=0;
        }  
    }

    if (key == 113) //q
    { 
        //printf("you pressed q\n");
        testLoadBinary();      
    }

}

/*****************************************/

int BuildPopupMenu (void)
{
    int menu;

    menu = glutCreateMenu (SelectFromMenu);
    glutAddMenuEntry ("About semraster\tl", MENU_ABOUT);
    glutAddMenuEntry ("Toggle polygon fill\tp", MENU_POLYMODE);
    glutAddMenuEntry ("Toggle texturing\tt", MENU_TEXTURING);
    glutAddMenuEntry ("Exit  ", MENU_EXIT);

    return menu;
}

/*****************************************/
void MouseButton(int button, int state, int x, int y)
{
    // Respond to mouse button presses.
    // If button1 pressed, mark this state so we know in motion function.

    if (button == GLUT_LEFT_BUTTON)
      {
        g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
        g_yClick = y - 3 * g_fViewDistance;
      }
}

/*****************************************/
void MouseMotion(int x, int y)
{
    // If button1 pressed, zoom in/out if mouse is moved up/down.

    if (g_bButton1Down)
      {
        g_fViewDistance = (y - g_yClick) / 3.0;
        if (g_fViewDistance < VIEWING_DISTANCE_MIN)
           g_fViewDistance = VIEWING_DISTANCE_MIN;
        glutPostRedisplay();
      }
}


/***************************************/

//top level opengl loop 
void spinningCubeDemo(int *argc, char** argv){
    
    int screenSize = 512; //defaults to 512
    if (argv[1]){
       screenSize = atoi(argv[1]);
    }

    printf("\n\nstarting up semraster in %i resolution.\n", screenSize);

    // you can find documentation at http://reality.sgi.com/mjk/spec3/spec3.html   
    glutInit(argc, argv);  

    //  Select type of Display mode:   
    //  Double buffer 
    //  RGBA color
    //  Alpha components supported 
    //  Depth buffer   

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(screenSize, screenSize);  //window size

    // the window starts at the upper left corner of the screen  
    glutInitWindowPosition(0, 0);  
     
    window = glutCreateWindow("SEM raster display"); //create an opengl window 

    //glutDisplayFunc(&DrawGLScene);//register display callback       
    glutDisplayFunc(&DrawGLScene3D);   

    //glutFullScreen(); // if you want fullscreen. 

    // Even if there are no events, redraw our gl scene.  
    //glutIdleFunc(&DrawGLScene);
    glutIdleFunc(&DrawGLScene3D);

    glutReshapeFunc(&ReSizeGLScene);  //register window resize callback 
    glutKeyboardFunc(&keyPressed);    // Register key pressed callback 
    
    InitGL(screenSize, screenSize); // Initialize window. 
    
    ///////////////////////////
    //test of BMP saving 
    //create_Image("generated1.bmp");  
    //create_Image2("generated2.bmp"); 
    
    glutMouseFunc (MouseButton);
    glutMotionFunc (MouseMotion);
  
    // Create our popup menu
    BuildPopupMenu ();
    glutAttachMenu (GLUT_RIGHT_BUTTON);

    ImageLoad("textures/generated1.bmp"     , imageloaded_bfr);
    ImageLoad("textures/generated3.bmp", imageloaded_bfr2);

    glutMainLoop();// Start Event Processing Engine   




}

/***************************************/

// top level opengl loop 
void flatImageDemo(int *argc, char** argv){
    
    int screenSize = 512; //defaults to 512
    if (argv[1]){
       screenSize = atoi(argv[1]);
    }

    printf("\n\nstarting up semraster in %i resolution.\n", screenSize);


    // you can find documentation at http://reality.sgi.com/mjk/spec3/spec3.html   
    glutInit(argc, argv);  

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(screenSize, screenSize);  //window size

    // the window starts at the upper left corner of the screen  
    glutInitWindowPosition(0, 0);  
     
    window = glutCreateWindow("SEM raster display"); //create an opengl window 

    glutDisplayFunc(&DrawGLScene);//register display callback       

    // Even if there are no events, redraw our gl scene.  
    glutIdleFunc(&DrawGLScene);

    glutReshapeFunc(&ReSizeGLScene);  //register window resize callback 
    glutKeyboardFunc(&keyPressed);    // Register key pressed callback 
    
    InitGL(screenSize, screenSize); // Initialize window. 
    
    ///////////////////////////
    //test of BMP saving 
    //create_Image("generated1.bmp");  
    //create_Image2("generated2.bmp"); 
    
    glutMouseFunc (MouseButton);
    glutMotionFunc (MouseMotion);
  
    // Create our popup menu
    BuildPopupMenu ();
    glutAttachMenu (GLUT_RIGHT_BUTTON);


    // attempt to turn off antialiasing 
    // glDisable(GL_BLEND);
    // glDisable(GL_DITHER);
    // glDisable(GL_POINT_SMOOTH);
    // glDisable(GL_LINE_SMOOTH);
    // glDisable(GL_POLYGON_SMOOTH);
    // glHint(GL_POINT_SMOOTH, GL_DONT_CARE);
    // glHint(GL_LINE_SMOOTH, GL_DONT_CARE);
    // glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
    // #define GL_MULTISAMPLE_ARB 0x809D
    // glDisable( GL_MULTISAMPLE_ARB) ;

    ImageLoad("textures/generated1.bmp", imageloaded_bfr);
    ImageLoad("textures/generated3.bmp", imageloaded_bfr2);

    glutMainLoop();// Start Event Processing Engine   
   
}



/***************************************/


int main(int argc, char **argv) 
{  
    
    // flatImageDemo(&argc, argv); //start up openGL 

    spinningCubeDemo(&argc, argv); //start up openGL 
    


    return 1;
}














/***************************************/

/*

// same as dynamicUpdateTextures but test of animation 
void animateTextures()
{
    Image *image;
    image = (Image *) malloc(sizeof(Image));  

    if (image == NULL) {
      printf("Error allocating space for image");
      exit(0);
    }

    //// 

    unsigned long size; 

    image->sizeX = img_usize;
    image->sizeY = img_vsize;

    size = image->sizeX * image->sizeY * 3;

    image->data = (char *) malloc(size);

    RGBType bgcolor;
    RGBType *pt_bgcolor = &bgcolor;

    pt_bgcolor->r = 45;
    pt_bgcolor->g = 50;
    pt_bgcolor->b = 50;


    RGBType linecolor;
    RGBType *pt_linecolor = &linecolor;
    pt_linecolor->g = 230;
    pt_linecolor->b = 130;

    
    RGBType bordercolor;
    RGBType *pt_bordercolor = &bordercolor;
    pt_bordercolor->b = 255;


    RGBType hitcolor;
    RGBType *pt_hitcolor = &hitcolor;
    pt_hitcolor->r = 200;
    pt_hitcolor->b = 128;

    fillbuffer24(image, pt_bgcolor);
    
    //origin is bottom left - grr 
    int tl[2] = { 0, 0 };
    int br[2] = { image->sizeX-2, image->sizeY-2 };
    draw_square( image, image->sizeX, tl, br, pt_bordercolor );
   
    //draw the pong bouncer 
    if(upos>(image->sizeX-(pong_size*2)))
    {
        draw_fill_square(  image,  upos, vpos, pong_size*2, pt_hitcolor  );
    }else{
        draw_fill_square(  image,  upos, vpos, pong_size, pt_linecolor  );
        
    }

    ////

    // Create Texture   
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

    free( image );

}
*/




/***************************************/
/*

void makeTestImage()
{
    char image_name[] = "diagnostic.bmp"; 

    RGBType bgcolor;
    bgcolor.r=255;
    bgcolor.g=45;
    bgcolor.b=30;

    RGBType linecol;
    linecol.r=0;
    linecol.g=0;
    linecol.b=0;

    int locwidth  = 100;
    int locheight = 100;

    //24 bit
    RGBType *pixels  = createBuffer24(locwidth, locheight);
    if (pixels==NULL)
    {
        printf("Error creating test image framebuffer\n");
        return;
    }

    fillbuffer24( pixels, locwidth, locheight, &bgcolor);
    saveBMP_24bit(pixels, image_name, locwidth, locheight );

    //32 bit 
    //RGBAType *pixels = createBuffer32(locwidth, locheight);        
    //fillbuffer32( pixels, locwidth, locheight, &bgcolor);
    //saveBMP_24bit(copyBuffer24(pixels, locwidth, locheight), image_name, locwidth, locheight );

    char title[] = "scanfidstest";
    //writePng32(pixels, image_name, locwidth, locheight, title);


    //cleanup_heap(row_pointers, h);   
    free(pixels); 

}

*/


/***************************************/
/*
// Load Bitmaps And Convert To Textures
void LoadGLTextures(char* filename, Image *pixels ) 
{   
    
    //Image *pixels;
    //pixels = (Image *) malloc(sizeof(Image));  

    //RGBType *pixels  = createBuffer24(100, 100); 

    if (pixels == NULL) {
      printf("Error allocating space for image");
      exit(0);
    }

    if (!ImageLoad(filename, pixels )) {
      exit(1);
    }        
    
    // Create Texture   
    glGenTextures(1, &texture[0]);

    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.

    glTexImage2D(GL_TEXTURE_2D, 0, 3, pixels->sizeX, pixels->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels->data);
    
    //free(pixels);

}
*/

/***************************************/

/*
void dynamicUpdateTextures()
{
    Image *loaded_pixels;
    loaded_pixels = (Image *) malloc(sizeof(Image));  

    if (loaded_pixels == NULL) {
      printf("Error allocating space for image");
      exit(0);
    }

    //example 1
    // if (!dynamicImage(loaded_pixels )) {
    //   exit(1);
    // }        

    //example 2
    if (!dynamicImage2(loaded_pixels )) {
      exit(1);
    }  

    // Create Texture   
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image, 
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, loaded_pixels->sizeX, loaded_pixels->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, loaded_pixels->data);

    free(loaded_pixels);
}
*/