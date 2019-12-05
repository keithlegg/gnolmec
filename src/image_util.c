
/*************************************************************/
/*
   image_util.c 
  
      playground for random image processing functions that do NOT include:
          
          -framebuffers  (use framebuffer.c)
          -loaders/saves (use bitmap_io.c)


   Copyright (C) 2019 Keith Legg (keithlegg23@gmail.com)

*/
/*************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include "framebuffer.h"

/***************************************************************/
/* 
  Cleanup LIBPNG related memory allocation. 
  (You still have to free your own buffers maually.)
*/



/*

#include <png.h>

void cleanup_heap( png_bytep* rowpt, int height)
{
    int x=0;int y =0;
    for (y=0; y<height; y++){
        free(rowpt[y]);
    }
    free(rowpt);
}

// void read_png_file(png_bytep *row_pointers, const char* file_name, int *w, int *h, png_structp png_ptr,
//                     png_infop info_ptr, png_byte bit_depth, png_byte color_type, int number_of_passes)
// {
//     char header[8];    // 8 is the maximum size that can be checked
//     int x=0;int y =0;
//     // open file and test for it being a png  
//     FILE *fp = fopen(file_name, "rb");
//     if (!fp)
//         abort_("[read_png_file] File %s could not be opened for reading", file_name);
//     fread(header, 1, 8, fp);
//     png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//     if (!png_ptr)
//         abort_("[read_png_file] png_create_read_struct failed");
//     info_ptr = png_create_info_struct(png_ptr);
//     if (!info_ptr)
//         abort_("[read_png_file] png_create_info_struct failed");
//     if (setjmp(png_jmpbuf(png_ptr)))
//         abort_("[read_png_file] Error during init_io");
//     png_init_io(png_ptr, fp);
//     png_set_sig_bytes(png_ptr, 8);
//     png_read_info(png_ptr, info_ptr);
//     *w = png_get_image_width(png_ptr, info_ptr);
//     *h = png_get_image_height(png_ptr, info_ptr);
//     color_type = png_get_color_type(png_ptr, info_ptr);
//     bit_depth = png_get_bit_depth(png_ptr, info_ptr);
//     number_of_passes = png_set_interlace_handling(png_ptr);
//     png_read_update_info(png_ptr, info_ptr);
//     if (setjmp(png_jmpbuf(png_ptr)))
//         abort_("[read_png_file] Error during read_image");
//     row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * *h);
//     for (y=0; y< *h; y++)
//         row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
//     png_read_image(png_ptr, row_pointers);
//     fclose(fp);
// }

void write_png_file(png_bytep *row_pointers, const char* file_name, int w, int h, png_structp png_ptr,
                    png_infop info_ptr, png_byte bit_depth, png_byte color_type)
{
        FILE *fp = fopen(file_name, "wb");
        if (!fp)
                abort_("[write_png_file] File %s could not be opened for writing", file_name);

        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if (!png_ptr)
                abort_("[write_png_file] png_create_write_struct failed");

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
                abort_("[write_png_file] png_create_info_struct failed");

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during init_io");

        png_init_io(png_ptr, fp);

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing header");

        png_set_IHDR(png_ptr, info_ptr, w, h,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_write_info(png_ptr, info_ptr);

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during writing bytes");

        png_write_image(png_ptr, row_pointers);

        if (setjmp(png_jmpbuf(png_ptr)))
                abort_("[write_png_file] Error during end of write");

        png_write_end(png_ptr, NULL);
         
        ////  moved heap cleanup to seperate function.   
        // for (y=0; y<height; y++)
        //        free(row_pointers[y]);
        //free(row_pointers);

        fclose(fp);
}

void write_buffer_png(RGBType *pixels, png_bytep *row_pointers, const char* file_name, int w, int h,
                      png_structp png_ptr, png_infop info_ptr, png_byte bit_depth, png_byte color_type)
{
   int x=0;int y =0;
   int  pixItr = 0;

   for (int y = 0; y < h; y++)
   {     
       png_byte* row   = row_pointers[y];

       for (int x = 0; x < w; x++)
       {  
           pixItr = (y * w) + x; //bmp data 
           png_byte* ptr = &(row[x*3]);  //png data 
           ptr[0] = pixels[pixItr].r;       
           ptr[1] = pixels[pixItr].g;
           ptr[2] = pixels[pixItr].b;
       }
   }

   write_png_file(row_pointers, file_name, w, h, png_ptr, info_ptr, bit_depth, color_type);
}

void write_buffer_png(RGBAType *pixels, png_bytep *row_pointers, const char* file_name, int w, int h,
                      png_structp png_ptr, png_infop info_ptr, png_byte bit_depth, png_byte color_type)
{
   int x=0;int y =0;
   int  pixItr = 0;

   for (int y = 0; y < h; y++)
   {     
       png_byte* row = row_pointers[y];

       for (int x = 0; x < w; x++)
       {  
           pixItr = (y * w) + x;  
           png_byte* ptr = &(row[x*4]);   

           ptr[0] = pixels[pixItr].r;       
           ptr[1] = pixels[pixItr].g;
           ptr[2] = pixels[pixItr].b;
           ptr[3] = pixels[pixItr].a;
       }
   }

   write_png_file(row_pointers, file_name, w, h, png_ptr, info_ptr, bit_depth, color_type);
}

//  Writes out a bit PNG image.
//  this function is self contained (decoupled) from the other two read/write PNG functions 

int writePng32(RGBType *pixels, char* filename, int w, int h, char* title)
{
    int code   = 0;
    int pixItr = 0;

    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep row;
    png_bytep col;
    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        code = 1;
        goto finalise;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }

    png_init_io(png_ptr, fp);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, w, h,
            8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // Set title
    if (title != NULL) {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "Title";
        title_text.text = title;
        png_set_text(png_ptr, info_ptr, &title_text, 1);
    }

    png_write_info(png_ptr, info_ptr);


    int x, y;

    // Allocate memory for one row (4 bytes per pixel - RGBA)
    row = (png_bytep) malloc(4 * w * sizeof(png_byte));

    // Write image data
    for (y=0 ; y<h ; y++) {
        for (x=0 ; x<w ; x++) {
            col = (png_bytep) &(row[x*4]);
            pixItr = (y * w) + x; 

            col[0] = pixels[pixItr].r ;
            col[1] = pixels[pixItr].g ;
            col[2] = pixels[pixItr].b ;
            col[3] = 255; 
        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);

    finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);

    return code;
}

int writePng32(RGBAType *pixels, char* filename, int w, int h, char* title)
{
    int code   = 0;
    int pixItr = 0;

    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep row;
    png_bytep col;
    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        code = 1;
        goto finalise;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }

    png_init_io(png_ptr, fp);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, w, h,
            8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // Set title
    if (title != NULL) {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "Title";
        title_text.text = title;
        png_set_text(png_ptr, info_ptr, &title_text, 1);
    }

    png_write_info(png_ptr, info_ptr);


    int x, y;

    // Allocate memory for one row (4 bytes per pixel - RGBA)
    row = (png_bytep) malloc(4 * w * sizeof(png_byte));

    // Write image data
    for (y=0 ; y<h ; y++) {
        for (x=0 ; x<w ; x++) {
            col = (png_bytep) &(row[x*4]);
            pixItr = (y * w) + x; 

            col[0] = pixels[pixItr].r ;
            col[1] = pixels[pixItr].g ;
            col[2] = pixels[pixItr].b ;
            col[3] = pixels[pixItr].a ; 
        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);

    finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);

    return code;
}


 // Writes out a bit PNG image.
 // this function is self contained (decoupled) from the other two read/write PNG functions 

int writePng24(RGBAType *pixels, char* filename, int w, int h, char* title)
{
    int code   = 0;
    int pixItr = 0;

    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep row;
    png_bytep col;
    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        code = 1;
        goto finalise;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }

    png_init_io(png_ptr, fp);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, w, h,
            8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // Set title
    if (title != NULL) {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "Title";
        title_text.text = title;
        png_set_text(png_ptr, info_ptr, &title_text, 1);
    }

    png_write_info(png_ptr, info_ptr);

    // Allocate memory for one row (4 bytes per pixel - RGBA)
    row = (png_bytep) malloc(3 * w * sizeof(png_byte));
       
    // Write image data
    int x, y;
    for (y=0 ; y<h ; y++) {
        for (x=0 ; x<w ; x++) {
            col = (png_bytep) &(row[x*3]);
            pixItr = (y * w) + x; 

            col[0] = pixels[pixItr].r ;
            col[1] = pixels[pixItr].g ;
            col[2] = pixels[pixItr].b ;
        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);

    finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);

    return code;
}
 
//  Writes out a 24 bit PNG image.
//  this function is self contained (decoupled) from the other two read/write PNG functions 

int writePng24(RGBType *pixels, char* filename, int w, int h, char* title)
{
    int code   = 0;
    int pixItr = 0;

    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep row;
    png_bytep col;
    // Open file for writing (binary mode)
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Could not open file %s for writing\n", filename);
        code = 1;
        goto finalise;
    }

    // Initialize write structure
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        fprintf(stderr, "Could not allocate write struct\n");
        code = 1;
        goto finalise;
    }

    // Initialize info structure
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fprintf(stderr, "Could not allocate info struct\n");
        code = 1;
        goto finalise;
    }

    // Setup Exception handling
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "Error during png creation\n");
        code = 1;
        goto finalise;
    }

    png_init_io(png_ptr, fp);

    // Write header (8 bit colour depth)
    png_set_IHDR(png_ptr, info_ptr, w, h,
            8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // Set title
    if (title != NULL) {
        png_text title_text;
        title_text.compression = PNG_TEXT_COMPRESSION_NONE;
        title_text.key = "Title";
        title_text.text = title;
        png_set_text(png_ptr, info_ptr, &title_text, 1);
    }

    png_write_info(png_ptr, info_ptr);

    // Allocate memory for one row (3 bytes per pixel - RGB)
    row = (png_bytep) malloc(3 * w * sizeof(png_byte));
       
    // Write image data
    int x, y;
    for (y=0 ; y<h ; y++) {
        for (x=0 ; x<w ; x++) {
            col = (png_bytep) &(row[x*3]);
            pixItr = (y * w) + x; 

            col[0] = pixels[pixItr].r ;
            col[1] = pixels[pixItr].g ;
            col[2] = pixels[pixItr].b ;

        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);

    finalise:
    if (fp != NULL) fclose(fp);
    if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL) free(row);

    return code;
}

int* read_png_fileinfo( const char* file_name, int width, int height, png_structp png_ptr, png_infop info_ptr, 
                        png_byte color_type, png_byte bit_depth )

//   open the PNG file - grab info about it and return just the data 
//   returns an array of [ width, height , coarse, max, channel depth, overall depth ]

{

    static int out[10];

    char header[8];    // 8 is the maximum size that can be checked

    // open file and test for it being a png  
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
            abort_("[read_png_file] File %s could not be opened for reading", file_name);
    fread(header, 1, 8, fp);

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
            abort_("[read_png_file] png_create_read_struct failed");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
            abort_("[read_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
            abort_("[read_png_file] Error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);


    int longedge =0;
    if (width>height){
        longedge = width;
    }else{
        longedge = height;
    }
    
    out[0] = width; 
    out[1] = height; 
   
    int* tmp = 0;

    //tmp = autoscan_settings(width, height);
    //out[2] = tmp[0];//coarse
    //out[3] = tmp[1];//max
    //printf("********************\n" );
    //printf("blur radius:      %i\n", calc_blur_radius(width, height) );
    //printf("scan settings:    coarse %i max %i \n", sc_coarse, sc_max);
    //printf("width             %i \n", width);    

    // PNG_COLOR_TYPE_GRAY       - (bit depths 1, 2, 4, 8, 16)
    // PNG_COLOR_TYPE_GRAY_ALPHA - (bit depths 8, 16)
    // PNG_COLOR_TYPE_PALETTE    - (bit depths 1, 2, 4, 8)
    // PNG_COLOR_TYPE_RGB        - (bit_depths 8, 16)
    // PNG_COLOR_TYPE_RGB_ALPHA  - (bit_depths 8, 16)
    // PNG_COLOR_MASK_PALETTE
    // PNG_COLOR_MASK_COLOR
    // PNG_COLOR_MASK_ALPHA
   
    
    if(color_type==PNG_COLOR_TYPE_RGB){
        out[4] = 24;
    }else if(color_type==PNG_COLOR_TYPE_RGB_ALPHA){
        out[4] = 32;
    }else{
        out[4] = -1; //undefined for now 
    }

    out[5] = bit_depth;
    out[6] = tmp[2];//shrinkage
    out[7] = tmp[3];//guassian passes
    out[8] = tmp[4];//enable prescale

    return out; //[width, height, coarse, max , 24/32 , bits per channel]

}

*/



/*
  This creates a test image for file exporitng , etc. It creates a Mandelbrot set "test" image. 

  -TO USE- 
  //createTestImage(pixels, width, height, -0.802, -0.177, 0.011, 110);

*/

inline void setRGB(RGBAType *ptr, float val)
{
    /* map a float value to an RGB color */

    int v = (int)(val * 767);
    if (v < 0) v = 0;
    if (v > 767) v = 767;
    int offset = v % 256;

    if (v<256) {
        ptr->r = 0; ptr->g = 0; ptr->b = offset;
    }
    else if (v<512) {
         ptr->r = 255-offset; ptr->g = (int)offset/2; ptr->b = 0;
    }
    else {
        ptr->r = offset; ptr->g = 255; ptr->b = 0;
    }
}

void createTestImage(RGBAType *buffer, int width, int height, float xS, float yS, float rad, int maxIteration)
{
    int xPos, yPos;
    float minMu = maxIteration;
    float maxMu = 0;

    for (yPos=0 ; yPos<height ; yPos++)
    {
        float yP = (yS-rad) + (2.0f*rad/height)*yPos;

        for (xPos=0 ; xPos<width ; xPos++)
        {
            float xP = (xS-rad) + (2.0f*rad/width)*xPos;

            int iteration = 0;
            float x = 0;
            float y = 0;

            while (x*x + y*y <= 4 && iteration < maxIteration)
            {
                float tmp = x*x - y*y + xP;
                y = 2*x*y + yP;
                x = tmp;
                iteration++;
            }

            if (iteration < maxIteration) {
                float modZ = sqrt(x*x + y*y);
                float mu = iteration - (log(log(modZ))) / log(2);
                if (mu > maxMu) maxMu = mu;
                if (mu < minMu) minMu = mu;

                RGBAType* pixPtr = &( buffer[(yPos * width) + xPos]);
                setRGB(pixPtr, ( (mu- minMu) / (maxMu - minMu) ) );
            }
            else {
                RGBAType* pixPtr = &( buffer[(yPos * width) + xPos]);
                setRGB(pixPtr, 0);
            }
        }
    }
}


