#ifndef BITMAP_IO_H    
#define BITMAP_IO_H

#include "framebuffer.h"



void saveBMP_24bit ( RGBType *data, const char *filename, int w, int h); 
void loadBMP_24bit( RGBType *data, const char *filename, int *w, int *h);

//int save_image(IMAGE *image, char name[]);






#endif
