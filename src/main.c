
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

#include "math_op.h"     // general math operations
#include "point_op.h"    // geometry operations
#include "framebuffer.h" // raster operations
#include "examples_fb.h" // example raster ops

#include "image_util.h"  // experimental features, etc 
#include "bitmap_io.h"   
#include "bitmap_cpu.h"  //highly experimental - dont ask 


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

static int g_yClick = 0;
static float g_lightPos[4] = { 10, 10, -100, 1 };   // Position of light



enum {
    MENU_ABOUT,
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


    glutPostRedisplay();
}


/***************************************************************/

int window_id; // The number of our GLUT window 
 
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



void animateTextures3(Image *loaded_texture)
{
    //test of new framebuffer commands
    RGBType* pt_rgb_bfr =  createBuffer24(loaded_texture->sizeX, loaded_texture->sizeY);    
    
    //copyBuffer24( loaded_texture ,  pt_rgb_bfr ); //convert "Image" to "RGBType"
    
    copyBuffer24( imageloaded_bfr2 ,  pt_rgb_bfr ); //convert "Image" to "RGBType"

    // 

    //make a color for some lines 
    RGBType line_color;
    RGBType *pt_linecolor = &line_color;
    pt_linecolor->r = 255;
    pt_linecolor->g = 0;
    pt_linecolor->b = 0;

    //make a color for some lines 
    RGBType line_color2;
    RGBType *pt_linecolor2 = &line_color2;
    pt_linecolor2->r = 155;
    pt_linecolor2->g = 255;
    pt_linecolor2->b = 0;

    // int cp_tl[2] = {0};
    // int cp_br[2] = {0};
    // cp_tl[0] = upos-pong_size;
    // cp_tl[1] = vpos-pong_size;
    // cp_br[0] = upos+pong_size;
    // cp_br[1] = vpos+pong_size;
    // draw_line(pt_rgb_bfr, loaded_texture->sizeX , cp_tl[0] , cp_tl[1] , cp_br[0] , cp_br[1] , pt_linecolor); 
    // draw_line(pt_rgb_bfr, loaded_texture->sizeX , cp_tl[0] , cp_tl[1] , cp_tl[0] , cp_br[1] , pt_linecolor);
    
    //just to show its alive - bounce a ball around 
    //draw_fill_circle ( pt_rgb_bfr, loaded_texture->sizeX, upos, vpos, pong_size, pt_linecolor);

    // const char *filename = "rgb_buffer.bmp";
    // saveBMP_24bit (pt_rgb_bfr, filename , image_x, image_y);
    // free(pt_image_bfr);

    ///////////////////////
    int x = 0;
    
    int num_cells = 20;
    int cell_size = (int)loaded_texture->sizeX/num_cells;
    
    for (x=cell_size;x<=loaded_texture->sizeX-cell_size;x=x+cell_size){
        draw_fill_circle ( pt_rgb_bfr, loaded_texture->sizeX, x, vpos, cell_size/2, pt_linecolor2);
    }


    ///////////////////////
    /*
    // first stab at some matrix and vector art 
    m33 identity = new_m33(1,0,0,
                           0,1,0,
                           0,0,1);

    vec2 v1 = newvec2(102.0, 301.0);

    draw_line(pt_rgb_bfr, loaded_texture->sizeX , v1.x , v1.y , 0 , 0 , pt_linecolor);
    */
    ///////////////////////

    //overwrite loaded buffer with lines
    copyBuffer24( pt_rgb_bfr, loaded_texture ); //convert "RGBType" to "Image"

    free(pt_rgb_bfr);
     
    // create and apply 2D texture   
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    glTexImage2D(GL_TEXTURE_2D, 0, 3, loaded_texture->sizeX, loaded_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, loaded_texture->data);

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
 
// blitBuffer32

void drawglscene_2d()
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
    
    //animateTextures2(main_bg_bfr);
    
    animateTextures3(main_bg_bfr);

    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
    
}//end display callback 


/***************************************/

void draw_3d_cube()
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

    
    //animateTextures2(main_bg_bfr);
    
    animateTextures3(main_bg_bfr);

    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
}

/***************************************/


void drawglscene_3d()
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

    
    //animateTextures2(main_bg_bfr);
    
    animateTextures3(main_bg_bfr);

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
        glutDestroyWindow(window_id); 
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



    if (key == 101) //e
    { 
        glutReshapeWindow(800, 800);
        glutPositionWindow(0,0);
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


void MouseMotion(int x, int y)
{
    // If button1 pressed, zoom in/out if mouse is moved up/down.

    if (g_bButton1Down)
    {
        
        // g_fViewDistance = (y - g_yClick) / 3.0;
        // if (g_fViewDistance < VIEWING_DISTANCE_MIN)
        //    g_fViewDistance = VIEWING_DISTANCE_MIN;
        // glutPostRedisplay();
    }
}


/*****************************************/
/*****************************************/

void maya_mouse_button(int button, int state, int x, int y)
{
    // Respond to mouse button presses.
    // If button1 pressed, mark this state so we know in motion function.

    printf("maya button x %d y %d \n",x , y);

    if (button == GLUT_LEFT_BUTTON)
      {
        g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
        g_yClick = y - 3 * g_fViewDistance;
        
        printf("maya left click \n");

      }

    if (button == GLUT_MIDDLE_BUTTON)
      {
        
        g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
        printf("maya middle click \n");

      }

    if (button == GLUT_RIGHT_BUTTON)
      {
        
        printf("maya right click \n");

      }

}


// var x = Input.GetAxis("Mouse X") ;
// var y = Input.GetAxis("Mouse Y") ;


void maya_mouse_motion(int x, int y)
{
    // If button1 pressed, zoom in/out if mouse is moved up/down.
 

    if (g_bButton1Down)
    {
        printf("MOVE! %d %d \n", x, y);
        
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
     
    window_id = glutCreateWindow("3D cube demo"); //create an opengl window 

    //register display callback       
    glutDisplayFunc(&drawglscene_3d);   

    // Even if there are no events, redraw our gl scene.  
    glutIdleFunc(&drawglscene_3d);

    glutReshapeFunc(&ReSizeGLScene);  //register window resize callback 
    glutKeyboardFunc(&keyPressed);    // Register key pressed callback 
    
    InitGL(screenSize, screenSize); // Initialize window. 
    
    ///////////////////////////
    //test of BMP saving 
    //create_Image("generated1.bmp");  
    //create_Image2("generated2.bmp"); 

    ///////////////////////////    
    //glutMouseFunc (MouseButton);
    //glutMotionFunc (MouseMotion);
  
    ///////////////////////////      
    // Create our popup menu
    BuildPopupMenu ();
    glutAttachMenu (GLUT_RIGHT_BUTTON);

    loadImage("textures/generated1.bmp" , imageloaded_bfr);
    loadImage("textures/generated3.bmp" , imageloaded_bfr2);

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
     
    window_id = glutCreateWindow("2D polygon demo"); //create an opengl window 

    glutDisplayFunc(&drawglscene_2d);//register display callback       

    // Even if there are no events, redraw our gl scene.  
    glutIdleFunc(&drawglscene_2d);

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
    
    /***********************/
    // 2d playground for framebuffer 

    loadImage("textures/generated1.bmp", imageloaded_bfr);
    loadImage("textures/generated3.bmp", imageloaded_bfr2);
    
    /*
    //test of new framebuffer commands
    RGBType* pt_rgb_bfr =  createBuffer24(imageloaded_bfr->sizeX, imageloaded_bfr->sizeY);    
    
    //copyBuffer24( imageloaded_bfr2 ,  pt_rgb_bfr ); //convert "Image" to "RGBType"

    //make a color for some lines 
    RGBType line_color;
    RGBType *pt_linecolor = &line_color;
    pt_linecolor->r = 255;
    pt_linecolor->g = 0;
    pt_linecolor->b = 0;

    // draw 4 lines - crisscross applesauce 
    //draw_line(pt_rgb_bfr, imageloaded_bfr->sizeX, 0  , 20  , 50  , 50  , pt_linecolor); 
    draw_line(pt_rgb_bfr, imageloaded_bfr->sizeX, 0  , 0   , 511 , 511 , pt_linecolor); 
    draw_line(pt_rgb_bfr, imageloaded_bfr->sizeX, 511, 0   , 0   , 511 , pt_linecolor); 
    //draw_line(pt_rgb_bfr, imageloaded_bfr->sizeX, 0  , 511 , 511 , 0   , pt_linecolor);

    // const char *filename = "rgb_buffer.bmp";
    // saveBMP_24bit (pt_rgb_bfr, filename , image_x, image_y);
    // free(pt_image_bfr);

    //overwrite loaded buffer with lines
    copyBuffer24( pt_rgb_bfr, imageloaded_bfr ); //convert "RGBType" to "Image"
   
    */


    //imageloaded_bfr2
    
    /***********************/

    glutMainLoop();// Start Event Processing Engine   
   
}


/********************************************/

float zoomSpeed    = 1.2f;
float moveSpeed    = 1.9f;
float rotateSpeed  = 4.0f;
vec3 startpos = newvec3(0.0, 130.0, 60.0);


void maya_navigation_demo(int *argc, char** argv){
    
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
     
    window_id = glutCreateWindow("Maya navigation demo"); //create an opengl window 

    //register display callback       
    glutDisplayFunc(&draw_3d_cube);   

    // Even if there are no events, redraw our gl scene.  
    glutIdleFunc(&draw_3d_cube);

    glutReshapeFunc(&ReSizeGLScene);  //register window resize callback 
    glutKeyboardFunc(&keyPressed);    // Register key pressed callback 
    
    InitGL(screenSize, screenSize); // Initialize window. 


    ///////////////////////////    
    glutMouseFunc (maya_mouse_button);
    glutMotionFunc (maya_mouse_motion);
  
    ///////////////////////////  



    // GameObject orbitVector ; (AKA Transform node in Maya )
    quaternion orbt_rot_original;
    vec3 orbt_xform_original;


    //     // Use this for initialization
    //     void Start () {
    //     // Create a capsule (which will be the lookAt target and global orbit vector)
    //     orbitVector = GameObject.CreatePrimitive(PrimitiveType.Capsule);
    //     orbitVector.transform.position = Vector3.zero;
    //     // Snap the camera to align with the grid in set starting position (otherwise everything gets a bit wonky)
    //     transform.position = startpos;  
    //     transform.LookAt(orbitVector.transform.position, Vector3.up);
    //     orbitVector.renderer.enabled = false; //hide the capsule object     
    //     ///
    //     orbt_xform_original = orbitVector.transform.position;
    //     orbt_rot_original   = orbitVector.transform.rotation;
    // }

    
    // void reset_xforms(){
    //     transform.parent = orbitVector.transform;
    //     orbitVector.transform.position = orbt_xform_original ;
    //     orbitVector.transform.rotation = orbt_rot_original;
    //     transform.parent = null;
    //     transform.position = startpos;
    // }


    
    //var wheelie = Input.GetAxis("Mouse ScrollWheel");
        
    // if (wheelie < 0) // back
    // {
    //     var currentZoomSpeed = 100f;
    //     transform.Translate(Vector3.forward * (wheelie * currentZoomSpeed));
    // }
    // if (wheelie > 0) // back
    // {
    //      var currentZoomSpeed = 100f;
    //      transform.Translate(Vector3.forward * (wheelie * currentZoomSpeed));
    // }

    /*
    //Input.GetAxis("Mouse ScrollWheel") < 0) // back
    if( Input.GetKey(KeyCode.RightAlt) || Input.GetKey(KeyCode.LeftAlt) ){

      // Distance between camera and orbitVector. We'll need this in a few places
      var distanceToOrbit = Vector3.Distance(transform.position, orbitVector.transform.position);
    
        //RMB - ZOOM
        if (Input.GetMouseButton(1)) {
            
            // Refine the rotateSpeed based on distance to orbitVector
            var currentZoomSpeed = Mathf.Clamp(zoomSpeed * (distanceToOrbit / 50), 0.1f, 2.0f);
            
            // Move the camera in/out
            transform.Translate(Vector3.forward * (x * currentZoomSpeed));
            
            // If about to collide with the orbitVector, repulse the orbitVector slightly to keep it in front of us
            if (Vector3.Distance(transform.position, orbitVector.transform.position) < 3) {
                orbitVector.transform.Translate(Vector3.forward, transform);
            }

        
        //LMB - PIVOT
        } else if (Input.GetMouseButton(0)) {
            
            // Refine the rotateSpeed based on distance to orbitVector
            var currentRotateSpeed = Mathf.Clamp(rotateSpeed * (distanceToOrbit / 50), 1.0f, rotateSpeed);
            
            
            // Temporarily parent the camera to orbitVector and rotate orbitVector as desired
            transform.parent = orbitVector.transform;
            orbitVector.transform.Rotate(Vector3.right * (y * currentRotateSpeed));
            orbitVector.transform.Rotate(Vector3.up * (x * currentRotateSpeed), Space.World);
            transform.parent = null;
                    
        //MMB - PAN
        else if (Input.GetMouseButton(2)) {
            
            // Calculate move speed
            var translateX = Vector3.right * (x * moveSpeed) * -1;
            var translateY = Vector3.up * (y * moveSpeed) * -1;
            
            // Move the camera
            transform.Translate(translateX);
            transform.Translate(translateY);
            
            // Move the orbitVector with the same values, along the camera's axes. In effect causing it to behave as if temporarily parented.
            orbitVector.transform.Translate(translateX, transform);
            orbitVector.transform.Translate(translateY, transform);
        }
    */        
            



    ///////////////////////////      
    // Create our popup menu
    //BuildPopupMenu ();
    //glutAttachMenu (GLUT_RIGHT_BUTTON);

    loadImage("textures/generated1.bmp" , imageloaded_bfr);
    loadImage("textures/generated3.bmp" , imageloaded_bfr2);

    glutMainLoop();// Start Event Processing Engine   


}

/***************************************/

void test_math_ops(void){

    vec2 vec2d = newvec2( 12.5, 32.6 );
    vec2 nrml_vec2 = normalize(vec2d);

    printf("-------------------------------------------------\n");

    printf("# 2d vector            %f %f     \n", vec2d.x, vec2d.y );
    printf("# 2d vector length is  %f        \n", fcalc_distance(vec2d) );
    printf("# 2d vector normalized %f %f     \n", nrml_vec2.x, nrml_vec2.y );

    printf("\n");

    /**********/

    vec3 vec3d      = newvec3( 12.5, 32.5, 42.4);
    vec3 nrml_vec3 = normalize(vec3d);

    printf("# 3d vector            %f %f %f  \n", vec3d.x, vec3d.y, vec3d.z );
    printf("# 3d vector length is  %f        \n", fcalc_distance(vec3d) );
    printf("# 3d vector normalized %f %f %f  \n", nrml_vec3.x, nrml_vec3.y, nrml_vec3.z );
    
    printf("-------------------------------------------------\n");    

    m33 new_m33 = test_indices33();
    m44 new_m44 = identity44();

   

    // print_matrix(transpose(new_m33));

    printf("-------------------------------------------------\n");  



}


/***************************************/


int main(int argc, char **argv) 
{  
    
    // test_framebuffer_funcs();
    
    test_math_ops();

    // flatImageDemo(&argc, argv); //start up openGL 
    //spinningCubeDemo(&argc, argv); //start up openGL 
    
    maya_navigation_demo(&argc, argv);



    return 1;
}

