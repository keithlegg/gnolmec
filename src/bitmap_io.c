/*************************************************************/
/*
   bitmap_io.c 
       -  file importer/ exporters - mostly image loaders and saver

   Copyright (C) 2019 Keith Legg (keithlegg23@gmail.com)

*/
/*************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "framebuffer.h"
#include "bitmap_io.h"




/***************************************/


/* 
   experimental file format exporter - works but still buggy 

   -TO USE -  

   RGBAType *pixels = read_png_create_buffer32( INFILE );
   RGBType *pixels2 =  cvt32bit_24bit(pixels, width, height);   
   saveBMP_24bit ( pixels2 , OUTFILE, width, height) ;
   free(pixels); free(pixels2);

*/


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
// Load a 24Bit BMP into an "Image" type object  

int loadImage(const char *filename, Image *image) 
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

    // reverse all of the colors. (bgr -> rgb)
    for (i=0;i<size;i+=3) { 
      temp = image->data[i];
      image->data[i] = image->data[i+2];
      image->data[i+2] = temp;
    }

    //free(image->data);
    return 1;
}
    

/***************************************/
// Save a 24Bit BMP from an "RGBType" object  

void saveBMP_24bit ( RGBType *data, const char *filename, int w, int h) {

    FILE *f;
    int k = w*h;
    int s = 4*k;
    int filesize = 54 +s;

    int dpi = 300;
    double factor = 39.375;

    int m = (int)factor;
    int ppm = dpi*m;

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0 ,0,0,0,0 , 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};

    bmpfileheader[2] = (unsigned char) (filesize);
    bmpfileheader[3] = (unsigned char) (filesize>>8);
    bmpfileheader[4] = (unsigned char) (filesize>>16);
    bmpfileheader[5] = (unsigned char) (filesize>>24);

    bmpinfoheader[4] = (unsigned char) (w);
    bmpinfoheader[5] = (unsigned char) (w>>8);
    bmpinfoheader[6] = (unsigned char) (w>>16);
    bmpinfoheader[7] = (unsigned char) (w>>24);

    bmpinfoheader[8]  = (unsigned char) (h);
    bmpinfoheader[9]  = (unsigned char) (h>>8);
    bmpinfoheader[10] = (unsigned char) (h>>16);
    bmpinfoheader[11] = (unsigned char) (h>>24);

    bmpinfoheader[21] = (unsigned char) (s);
    bmpinfoheader[22] = (unsigned char) (s>>8);
    bmpinfoheader[23] = (unsigned char) (s>>16);
    bmpinfoheader[24] = (unsigned char) (s>>24);

    bmpinfoheader[25] = (unsigned char) (ppm);
    bmpinfoheader[26] = (unsigned char) (ppm>>8);
    bmpinfoheader[27] = (unsigned char) (ppm>>16);
    bmpinfoheader[28] = (unsigned char) (ppm>>24);

    bmpinfoheader[29] = (unsigned char) (ppm);
    bmpinfoheader[30] = (unsigned char) (ppm>>8);
    bmpinfoheader[31] = (unsigned char) (ppm>>16);
    bmpinfoheader[32] = (unsigned char) (ppm>>24);

    f = fopen( filename,"wb");
    fwrite( bmpfileheader, 1, 14, f);
    fwrite( bmpinfoheader, 1, 40, f);

    for (int i = 0; i < k;i++){
       RGBType *rgb = &(data[i]);

       double red   = rgb->r*-255;  // debug - keith flipped these but may have broken something 
       double green = rgb->g*-255;  // debug - keith flipped these but may have broken something 
       double blue  = rgb->b*-255;  // debug - keith flipped these but may have broken something 

       unsigned char color[3] = { (int)floor(blue),(int)floor(green),(int)floor(red) };
       fwrite (color, 1,3,f);
    }
    fclose(f);

}


/************************************/
// Load a 24Bit BMP from an "RGBType" object  
// DEBUG - UNTESTED AND NOT DONE 
// WE HAVE DATA IN MEMORY BUT THATS ALL 

void loadBMP_24bit( RGBType *data, const char *filename, int *w, int *h){
    FILE *file;
    unsigned long size;                 // size of the image in bytes.
    unsigned long i;                    // standard counter.
    unsigned short int planes;          // number of planes in image (must be 1) 
    unsigned short int bpp;             // number of bits per pixel (must be 24)
    char temp;                          // temporary color storage for bgr-rgb conversion.
   
    char *imageData = 0;  // temporary color storage

    if ((file = fopen(filename, "rb"))==NULL){printf("File Not Found : %s\n",filename);}
    
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);

    // read the width (and set value via pointer)
    if ((i = fread(w, 4, 1, file)) != 1) {printf("Error reading width from %s.\n", filename);}
    printf("Width of %s: %i\n", filename, *w);
    
    // read the height (and set value via pointer)
    if ((i = fread(h, 4, 1, file)) != 1) {printf("Error reading height from %s.\n", filename);}
    printf("Height of %s: %i\n", filename, *h);
    
    // calculate the size (assuming 24 bits or 3 bytes per pixel).
    size = *w * *h *3;

    // read the planes
    if ((fread(&planes, 2, 1, file)) != 1) {printf("Error reading planes from %s.\n", filename);}
    if (planes != 1) {printf("Planes from %s is not 1: %u\n", filename, planes);}

    // read the bpp
    if ((i = fread(&bpp, 2, 1, file)) != 1){printf("Error reading bpp from %s.\n", filename);}
    if (bpp != 24){ printf("Bpp from %s is not 24: %u\n", filename, bpp);}
    
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);

    // read the data. 
    imageData = (char *) malloc(size);
    if (imageData == NULL){ printf("Error allocating memory for color-corrected image data");}
    if ((i = fread(imageData, size, 1, file)) != 1) {printf("Error reading image data from %s.\n", filename);}

    
    //at this point "imagedata" contains the data 

    // reverse all of the colors. (bgr -> rgb)
    for (i=0;i<size;i+=3) { 
        temp = imageData[i];

        //printf("\\%02hhx", (unsigned char) temp ); //view char as hex
        //RGBType *pt_rgb = &( data [(y*w)+x]);
        
        //pixItr1 = &( pixels  [(y*w)+x] );
        //pixItr2 = &( output  [(y*w)+x] );

        //data[i]   = image->data[i+2];
        //data[i+2] = temp;

    }


    //fclose
    //free

}
   


/****************************************************************/

// FROM https://cboard.cprogramming.com/c-programming/110564-saving-bmp-image-files.html
// The RGBQUAD structure is still part of the BMP (DIB) file format even though it is, for the most part, not used for 16, 24 and 32 bit image formats.
// But keep in mind that the color table can be used for the high color and true color image formats.

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

/* 
#define byte unsigned char
#define dword unsigned int 
#define word unsigned short int 
 
#pragma pack (1)
 
//4 Bytes per each pallete color
typedef struct {
    unsigned char rgbBlue,    
                 rgbGreen,    
                   rgbRed,    
              rgbReserved;    
}BITMAP_PALETTE;
 
// BMP File header
typedef struct bitmap_file_header {
    byte bfType[2];    
    dword bfSize;
    word bfReserved1;    
    word bfReserved2;
    dword bfOffBits;
} BITMAP_FILE_HEADER;
 
// BMP Image header
typedef struct bitmap_info_header {
    dword biSize;
    dword biWidth;
    dword biHeight;
    word biPlanes;                   
    word biBitCount;
    dword biCompression;       
    dword biSizeImage;
    dword biXPelsPerMeter;     
    dword biYPelsPerMeter;
    dword biClrUsed;               
    dword biClrImportant;
} BITMAP_INFO_HEADER;
 
typedef struct image
{
    BITMAP_FILE_HEADER file_info;
    BITMAP_INFO_HEADER bitmap_info;
    BITMAP_PALETTE *bitmap_palette;
    byte *data;
} IMAGE;
 
int save_image(IMAGE *image, char name[])
{
    FILE *fp;
    int i, rowsize;
    byte temp;
    unsigned int row, col, colour;
    int nx, ny;
 
    if(image == NULL)
        return EXIT_FAILURE;
    fp=fopen(name, "wb");
    if(fp==NULL)
    {
        printf("cannot open the file %s\n", name);
        return EXIT_FAILURE;
    }

    fwrite(&image->file_info, sizeof(BITMAP_FILE_HEADER),1,fp);
    fwrite(&image->bitmap_info, sizeof(BITMAP_INFO_HEADER),1,fp);
    int itotalPalettbytes = (int) image->file_info.bfOffBits - sizeof(BITMAP_FILE_HEADER) - (int)sizeof(BITMAP_INFO_HEADER);
    fwrite((void *)(&image->bitmap_palette),4, itotalPalettbytes / 4, fp);
    if (((image->bitmap_info.biWidth * 3) % 4) == 0) {
        nx = image->bitmap_info.biWidth * 3;
    } 
    else {
        nx = image->bitmap_info.biWidth * 3 + 4 -((image->bitmap_info.biWidth * 3) % 4);
    }
    
    ny = image->bitmap_info.biHeight;
    fwrite((void *)image->data, sizeof(byte),nx * ny ,  fp);
    fclose(fp);
    return EXIT_SUCCESS;
}

void export_tool(int argc, char **argv)
{
    int iNx = 9, iNy;
    FILE *fp;
    int iTotalPaletteBytes;
    unsigned short int iType;     //holds the first two bytes of the file
    IMAGE *ImageInput = (IMAGE *) malloc(sizeof(IMAGE));
    if ((fp = fopen(argv[1], "rb")) == NULL)
    {
        printf("%s doesn't exist.\n",argv[1]);
    } 
    else
    {
        fread((void *)&iType, 2, 1, fp);
        //The BM is a short integer... 19778
        if (iType == 19778)
        {
            fseek(fp,0,SEEK_SET);
            //dump the first 14 bytes
            fread((void *)&(ImageInput->file_info), sizeof(BITMAP_FILE_HEADER), 1, fp);
            //dump the next 40 bytes
            fread((void *)&(ImageInput->bitmap_info), sizeof(BITMAP_INFO_HEADER), 1, fp);
            iTotalPaletteBytes = (int)ImageInput->file_info.bfOffBits - sizeof(BITMAP_FILE_HEADER) - (int)sizeof(BITMAP_INFO_HEADER);
            //Dimention the array
            ImageInput->bitmap_palette = (BITMAP_PALETTE *) malloc(iTotalPaletteBytes); 
            //dump the next 4*n bytes 
            fread((void *)(ImageInput->bitmap_palette),4, iTotalPaletteBytes / 4, fp);    //works same way
            if (ImageInput->bitmap_info.biBitCount == 24) {
                //Go to the beginning of the pixel data in the file
                fseek(fp, ImageInput->file_info.bfOffBits, SEEK_SET);
 
                //Calculate the width of the bitmap taking into account padding bytes
                if (((ImageInput->bitmap_info.biWidth * 3) % 4) == 0) {
                    iNx = ImageInput->bitmap_info.biWidth * 3;
                } 
                else {
                    iNx = ImageInput->bitmap_info.biWidth * 3 + 4 -((ImageInput->bitmap_info.biWidth * 3) % 4);
                }
                iNy = ImageInput->bitmap_info.biHeight;
                ImageInput->data = (unsigned char *) malloc(iNx * iNy * 3);    
                memset(ImageInput->data,0, sizeof ImageInput->data);
                //Retrive the data array from the file
                fread((void *)ImageInput->data, iNx * iNy * 3, 1, fp);
                save_image(ImageInput, "Test.bmp");
            }
        }
    }
   
}
*/


/************************************/

/*
    C# example - may be something to learn from 

    using System.Drawing.Imaging;
    using System.Runtime.InteropServices;
    ...

    public static Bitmap BitmapTo1Bpp(Bitmap img) {
      int w = img.Width;
      int h = img.Height;
      Bitmap bmp = new Bitmap(w, h, PixelFormat.Format1bppIndexed);
      BitmapData data = bmp.LockBits(new Rectangle(0, 0, w, h), ImageLockMode.ReadWrite, PixelFormat.Format1bppIndexed);
      byte[] scan = new byte[(w + 7) / 8];
      for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
          if (x % 8 == 0) scan[x / 8] = 0;
          Color c = img.GetPixel(x, y);
          if (c.GetBrightness() >= 0.5) scan[x / 8] |= (byte)(0x80 >> (x % 8));
        }
        Marshal.Copy(scan, 0, (IntPtr)((long)data.Scan0 + data.Stride * y), scan.Length);
      }
      bmp.UnlockBits(data);
      return bmp;
    }
*/

/************************************/