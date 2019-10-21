
/*************************************************************/
/*

    Example code to use as a copy/paste repository and 
    for testing various framebuffer functions 


*/
/*************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "examples_fb.h" 
#include "framebuffer.h" 
#include "image_util.h" 
#include "bitmap_io.h" 



typedef int BOOL;
#define TRUE 1
#define FALSE 0

/***************************************/

void test_framebuffer_funcs(void)
{

    int image_x = 512;
    int image_y = 512;

    bool test_rgb_type   = TRUE;
    bool test_rgba_type  = TRUE;
    bool test_image_type = TRUE;

    /***********************/
    /***********************/

    // Test the RGBType
    if (test_rgb_type){
   
        RGBType color;
        RGBType *pt_color = &color;
        pt_color->r = 60;
        pt_color->g = 60;
        pt_color->b = 60;

        
        RGBType line_color;
        RGBType *pt_linecolor = &line_color;
        pt_linecolor->r = 200;
        pt_linecolor->g = 0;
        pt_linecolor->b = 0;

        // create a 24 bit image with RGBType type 
        RGBType* pt_rgb_bfr =  createBuffer24(image_x, image_y);

        /*******/
        // fill solid color 
        fillbuffer24(pt_rgb_bfr, image_x, image_y, pt_color);

        /*******/
        // make a copy of it 
        RGBType* pt_cpyrgb_bfr = copyBuffer24( pt_rgb_bfr , image_x, image_y);

        /*******/
        // Bitdepth conversion  
        //RGBType *pixels2 =  cvt32bit_24bit(pixels, width, height);   

        /*******/
        // TESTED - Load an "Image" type, convert to RGBType, save it  
        // create an Image type buffer  (so we can test conversion to RGBType and save)
        // Image* pt_image_bfr = createBufferImage(image_x, image_y);
        // // load an image into memory (DEBUG WONT WORK IF DISK BITMAP IS WRONG SIZE)
        // const char *filename2 = "textures/generated1.bmp";        
        // loadImage(filename2, pt_image_bfr);
        // //convert "Image" to "RGBType"
        // copyBuffer24( pt_image_bfr ,  pt_rgb_bfr );
        // // save the 24 bit image 
        // const char *filename = "rgb_buffer.bmp";
        // saveBMP_24bit (pt_rgb_bfr, filename , image_x, image_y);
        // free(pt_image_bfr);

        /*******/
        // // TESTED-ISH - Load an Image, convert to RGBtype, draw a line on it and save 
        // Image* pt_image_bfr = createBufferImage(image_x, image_y);
        // // load an image into memory (DEBUG WONT WORK IF DISK BITMAP IS WRONG SIZE)
        // const char *filename2 = "textures/generated1.bmp";   
        // loadImage(filename2, pt_image_bfr);
        // copyBuffer24( pt_image_bfr ,  pt_rgb_bfr ); //convert "Image" to "RGBType"
        // draw_line(pt_rgb_bfr, image_x, 0, 20, 50, 50, pt_linecolor); 
        // const char *filename = "rgb_buffer.bmp";
        // saveBMP_24bit (pt_rgb_bfr, filename , image_x, image_y);
        // free(pt_image_bfr);


        /*******/
        // cleanup memory 
        free(pt_rgb_bfr);
        free(pt_cpyrgb_bfr);


    }


    /***********************/
    /***********************/
    // Test the RGBAType
    if (test_rgba_type){

        // create a 32 bit image with RGBAType type 
        RGBAType* pt_rgba_bfr =  createBuffer32(image_x, image_y);


        //void createTestImage(RGBAType *buffer, int width, int height, float xS, float yS, float rad, int maxIteration)
        createTestImage(pt_rgba_bfr, image_x, image_y,  -0.802, -0.177, 0.011, 110);

        //copyBuffer24
        // save the 24 bit image 
        //const char *filename = "rgb_buffer.bmp";
        //saveBMP_24bit (pt_rgb_bfr, filename , image_x, image_y);

    }

    /***********************/
    /***********************/
    if (test_image_type){
        // Test the Image type 
        
        //create a buffer 
        Image* pt_image_bfr = createBufferImage(image_x, image_y);

        //load an image into memory 
        const char *filename2 = "textures/generated3.bmp";        
        loadImage(filename2, pt_image_bfr);


        //RGBType = newRgb(255, 128, 0);
        //void fillbuffer24(Image *imgBuffer, RGBType *color)

        free(pt_image_bfr);

    }




}

