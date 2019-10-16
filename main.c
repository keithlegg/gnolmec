
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
static int g_Width = 600;                          // Initial window width
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

int steps[8] = {1024,512,256,128,64,32,16,8};
int step_idx = 2;


// moved to framebuffer.h 
// typedef struct Image{
//     unsigned int sizeX;
//     unsigned int sizeY;
//     char *data;
// } Image ;


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


/***************************************/

//int ImageLoad(char *filename, RGBType *pixels, int width, int height) 

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
    image->data = (char *) malloc(size);
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

    return 1;
}

//second example to use a builtin util to fill a buffer with color 
int dynamicImage2(Image *image) 
{
    unsigned long size; 

    image->sizeX = 256;
    image->sizeY = 256;

    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = image->sizeX * image->sizeY * 3;

    //printf("#### image mem size is %lu \n", size );
    image->data = (char *) malloc(size);
    //printf("## Image size is %u %u \n", image->sizeX, image->sizeY);

    RGBType *color  = createBuffer24( image->sizeX, image->sizeY ); 
    color->r = 0;
    color->g = 0;
    color->b = 255;

    //printf("## color is %i %i %i \n", color->r, color->g, color->b);
    
    fillbuffer24(image, color);


    return 1;
}



void dynamicUpdateTextures()
{
    Image *pixels;
    pixels = (Image *) malloc(sizeof(Image));

    if (pixels == NULL) {
      printf("Error allocating space for image");
      exit(0);
    }

    //example 1
    // if (!dynamicImage(pixels )) {
    //   exit(1);
    // }        

    //example 2
    if (!dynamicImage2(pixels )) {
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

}

/***************************************/

// Load Bitmaps And Convert To Textures
void LoadGLTextures(char* filename) 
{   
    
    Image *pixels;
    pixels = (Image *) malloc(sizeof(Image));
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
    glLoadIdentity();               // Reset The View

    glTranslatef(0.0f, 0.0f, -3.7f);    // move 5 units into the screen.

    glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.

    //DEFINE GEOMETRY HERE 
    glBegin(GL_QUADS);//draw a four sided polygon(s)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // BL texture and quad
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // BR texture and quad
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // TR texture and quad
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // TL texture and quad
    glEnd(); // done with the polygon.
    

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

    xrot+=15.0f;                        // X Axis Rotation  
    yrot+=15.0f;                        // Y Axis Rotation
    zrot+=15.0f;                        // Z Axis Rotation

    usleep(100000); 
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
        printf("you pressed a\n");  
    }
 
    if (key == 98) //b
    { 
        printf("you pressed b\n");
    }
  
    if (key == 99) //c
    { 
        printf("you pressed c\n");
        LoadGLTextures("textures/generated1.bmp");       
    }

    if (key == 100) //d
    { 
        printf("you pressed d\n");
        LoadGLTextures("textures/generated2.bmp");       
    }

    if (key == 102) //f
    { 
        printf("you pressed f\n");
        glutFullScreen();
    }

    if (key == 119) //w
    { 
        printf("you pressed w\n");
        //makeTestImage();       
        dynamicUpdateTextures();
    }

    if (key == 113) //q
    { 
        printf("you pressed q\n");
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

    glutMainLoop();// Start Event Processing Engine   
   
}



/***************************************/


int main(int argc, char **argv) 
{  
    
    //spinningCubeDemo(&argc, argv); //start up openGL 
    
    flatImageDemo(&argc, argv); //start up openGL 

    return 1;
}

