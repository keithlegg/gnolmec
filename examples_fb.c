#include <stdio.h>
#include <stdlib.h>

#include "examples_fb.h" 
#include "framebuffer.h" 
#include "image_util.h" 
  


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

        pt_color->r = 125;
        pt_color->g = 30;
        pt_color->b = 40;


        // create a 24 bit image with RGBType type 
        RGBType* pt_rgb_bfr =  createBuffer24(image_x, image_y);

        // fill solid color 
        fillbuffer24(pt_rgb_bfr, image_x, image_y, pt_color);

        // make a copy of it 
        RGBType* pt_cpyrgb_bfr = copyBuffer24( pt_rgb_bfr , image_x, image_y);


        //RGBAType *pixels = read_png_create_buffer32( INFILE );
        //RGBType *pixels2 =  cvt32bit_24bit(pixels, width, height);   


        //void loadBMP_24bit( RGBType *data, const char *filename, int *w, int *h){
        const char *filename2 = "textures/generated3.bmp";
        int loaded_w = 0;
        int loaded_h = 0;
        int *pt_w = &loaded_w;
        int *pt_h = &loaded_h;

        loadBMP_24bit( pt_cpyrgb_bfr, filename2, pt_w, pt_h);



        // save the 24 bit image 
        const char *filename = "rgb_buffer.bmp";
        saveBMP_24bit (pt_cpyrgb_bfr, filename , image_x, image_y);

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
        Image* pt_image_bfr = createBufferImage(image_x, image_y);
        //RGBType = newRgb(255, 128, 0);
        //void fillbuffer24(Image *imgBuffer, RGBType *color)

        free(pt_image_bfr);

    }




}

