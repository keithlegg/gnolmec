
#include <stdlib.h>
#include <stdio.h>

#include <stdarg.h>
#include <unistd.h>      


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


//to get on OSX 'ls /dev/tty.*'  
//#define SERIAL_PORT_ID "/dev/tty.usbserial-FTWWRSIBA" // "/dev/tty.usbserial-A400gnmx" // "/dev/ttyUSB0"
//#define OUTPUT_BINARY "/Users/klegg/image.bin"

#include "gl_setup.h"    // common to all - moved to reduce size 
#include "math_op.h"     // general math operations
#include "point_op.h"    // geometry operations
#include "framebuffer.h" // raster operations
#include "examples_fb.h" // example raster ops

#include "image_util.h"  // experimental features, etc 

#include "bitmap_io.h"   
//#include "bitmap_cpu.h"  //highly experimental - dont ask 
   

#include "flat_image_demo.h"
#include "cube_demo.h"
#include "maya_demo.h"

///////////////

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

 

///////////////

// int steps[8] = {1024,512,256,128,64,32,16,8};
// int step_idx = 2;
int use_tex = 0;

Image* main_bg_bfr      = createBufferImage(512,512); 
Image* imageloaded_bfr  = createBufferImage(512,512); 
Image* imageloaded_bfr2 = createBufferImage(512,512); 

static enum {
    MENU_ABOUT,
    MENU_POLYMODE,
    MENU_TEXTURING,
    MENU_EXIT
};


/***************************************/
/*
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
*/

/********************************************/


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

    //m33 new_m33 = test_indices33();
    //m44 new_m44 = identity44();

    quaternion quat = quaternion_identity();
    quaternion *p_quat = &quat;
    
    //quaternion_fr_euler(p_quat, 90.0, 0, 0 );

    //quaternion_rotx(p_quat, 42.2864 );
    
    //print_quaternion( quat );
    
    printf("mag %f \n", quaternion_mag(p_quat) );

    // print_matrix(transpose(new_m33));

    printf("-------------------------------------------------\n");  



}


/***************************************/


int main(int argc, char **argv) 
{  
    
    // test_framebuffer_funcs();
    
    test_math_ops();

    // flatImageDemo(&argc, argv); //start up openGL 
    // spinningCubeDemo(&argc, argv); //start up openGL 
    maya_navigation_demo(&argc, argv);

    return 1;
}

