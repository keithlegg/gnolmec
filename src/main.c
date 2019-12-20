/*****************************************************************/
    /*

        Gnolmec - GNOlmec. Its not Maya and I built it myself.
        Keith Legg, 2019 

    */
/*****************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <stdarg.h>
#include <unistd.h>      

/********************************************/
//OSX related 
#include <string.h>      //for memset 


//to get on OSX 'ls /dev/tty.*'  
//#define SERIAL_PORT_ID "/dev/tty.usbserial-FTWWRSIBA" // "/dev/tty.usbserial-A400gnmx" // "/dev/ttyUSB0"
//#define OUTPUT_BINARY "/Users/klegg/image.bin"


#include "gl_setup.h"        // common to all - moved to reduce size 
#include "math_op.h"         // general math operations
#include "point_op.h"        // geometry operations
#include "framebuffer.h"     // raster operations
#include "image_util.h"      // experimental playground - not much here 
#include "bitmap_io.h"       // load/save bitmaps to disk  

#include "obj_model.h" 
//#include "examples_fb.h"     // example raster ops
//#include "bitmap_cpu.h"  //highly experimental - dont ask 

#include "demo_flat.h"     // 2D openGL demo 
#include "demo_cube.h"     // 3D openGL demo  
#include "gnolmec.h"       // 3D maya navigation dmeo (porting a tool in from Unity3D )  


/*******************************************************/

/*
#include <sys/time.h>

void timer_test(){
    //Test of timer 
    timeval startCount;
    float foo = gettimeofday(&startCount, NULL);
    printf("%ld.%06ld\n", startCount.tv_sec, startCount.tv_usec);

};
*/



/*******************************************************/


int window_id; // The number of our GLUT window 
 
GLuint texture[3]; // storage for one texture  


// window properties
int scr_size_x      = 512;
int scr_size_y      = 512;
bool scr_full_toglr = TRUE;

/*******************************************************/

// data for animated "pong" texture 
unsigned int upos = 100;
unsigned int vpos = 320;  
unsigned int dir_u = 0;
unsigned int dir_v = 0;
unsigned int pong_speed = 5;
unsigned int pong_size  = 60;
unsigned int img_usize  = 512;
unsigned int img_vsize  = 512;

/*******************************************************/
int use_tex = 0;


Image* main_bg_bfr      = createBufferImage(512,512); 
Image* imageloaded_bfr  = createBufferImage(512,512); 
Image* imageloaded_bfr2 = createBufferImage(512,512); 


enum {
    MENU_ABOUT,
    MENU_POLYMODE,
    MENU_TEXTURING,
    MENU_EXIT
};

/*******************************************************/



void test_math_ops(void){

    vec2 vec2d = newvec2( 12.5, 32.6 );
    vec2 nrml_vec2 = normalize(vec2d);

    printf("-------------------------------------------------\n");

    printf("# 2d vector            %f %f     \n", vec2d.x, vec2d.y );
    printf("# 2d vector length is  %f        \n", length(vec2d) );
    printf("# 2d vector normalized %f %f     \n", nrml_vec2.x, nrml_vec2.y );

    printf("\n");

    /**********/

    vec3 vec3d      = newvec3( 12.5, 32.5, 42.4);
    vec3 nrml_vec3 = normalize(vec3d);

    printf("# 3d vector            %f %f %f  \n", vec3d.x, vec3d.y, vec3d.z );
    printf("# 3d vector length is  %f        \n", length(vec3d) );
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
 


void makeTestImage()
{
    char image_name[] = "diagnostic.bmp"; 

    RGBType bgcolor;
    bgcolor.r=45;
    bgcolor.g=45;
    bgcolor.b=30;

    RGBType linecol;
    linecol.r=255;
    linecol.g=0;
    linecol.b=0;

    int locwidth  = 512;
    int locheight = 512;

    //24 bit
    RGBType *pixels  = createBuffer24(locwidth, locheight);
    if (pixels==NULL)
    {
        printf("Error creating test image framebuffer\n");
        return;
    }

    fillbuffer24( pixels, locwidth, locheight, &bgcolor);

    //  draw_point ( Image *imgBuffer, int imagewidth, int xcoord, int ycoord, RGBType *color  );
    //  draw_point ( RGBAType *fb_image, int imagewidth, int pt[2], int color[3] );//untested 
    //  draw_point ( RGBAType *fb_image, int imagewidth, int xcoord, int ycoord, int color[3]  );
    //  draw_point ( RGBAType *fb_image, int imagewidth, int xcoord, int ycoord, RGBType *color  );

    //  draw_point ( RGBType *fb_image , int imagewidth, int xcoord, int ycoord, int color[3]  );
    //  draw_point ( RGBType *fb_image , int imagewidth, int xcoord, int ycoord, RGBType *color  );
    //  draw_point ( RGBType *fb_image , int imagewidth, int xcoord, int ycoord, int dia, RGBType *color  );

    draw_point ( pixels , locwidth, 1, 1, &linecol  );
    draw_point ( pixels , locwidth, 5, 5, &linecol  );

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

char* obj_filepath;  

int main(int argc, char **argv) 
{  

    char runmode[10];

    //printf("# test # %s %s \n", argv[0], argv[1], argv[2] ); 
    if( argc == 1 ) {    
        spinningCubeDemo(&argc, argv);
    }

    strcpy(runmode, "-v");
    if( strcmp(argv[1], runmode) == 0)
    {
        glutInit(&argc, argv);
        glutCreateWindow("GLUT");
        printf("#OpenGL version %s \n", glGetString(GL_VERSION) ) ;
        exit(0);
    }
    
    /********************/

    strcpy(runmode, "cube");
    if( strcmp(argv[1], runmode) == 0)
    {
        spinningCubeDemo(&argc, argv); 
    }    

    /********************/
    strcpy(runmode, "flat");
    if( strcmp(argv[1], runmode) == 0)
    {
        flatImageDemo(&argc, argv); 
    }

    /********************/

    strcpy(runmode, "vectorize");
    if( strcmp(argv[1], runmode) == 0)
    {
        // flatImageDemo(&argc, argv); 
    }

    /********************/
    
    if( argc == 2 ) {
        obj_filepath = argv[1] ;

        if( access( obj_filepath, F_OK ) != -1 ) {
            olmec(&argc, argv); 
        } else {
            printf("# file %s does not exist.\n", obj_filepath);
            exit(0);
        }

         
    }
 
    //test_framebuffer_funcs();
    
    // test_math_ops();

    //show_object();

    return 1;
}

