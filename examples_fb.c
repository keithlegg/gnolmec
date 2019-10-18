#include <stdio.h>
#include <stdlib.h>

#include "examples_fb.h" 
#include "framebuffer.h" 
#include "image_util.h" 
  



/***************************************/

void test_framebuffer_funcs(void){


    int image_x = 512;
    int image_y = 512;

    RGBType* pt_rgb_bfr =  createBuffer24(image_x, image_y);

    const char *filename = "rgb_buffer.bmp";
    saveBMP_24bit (pt_rgb_bfr, filename , image_x, image_y);


    Image* pt_image_bfr = createBufferImage(image_x, image_y);
    //RGBType = newRgb(255, 128, 0);
    

    free(pt_rgb_bfr);
    free(pt_image_bfr);

}

