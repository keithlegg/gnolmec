/*****************************************************************************/
    /* 
        obj_model.c
        
            Load and save 3D models from disk 


        Copyright (C) 2019 Keith Legg  (keithlegg23@gmail.com)
    
    */
         
/*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "obj_model.h"

struct obj_model loader;



void load_objfile( char *filepath)
{

   FILE *fp;
   int c;
   
   int i = 0;

   fp = fopen(filepath,"r");
   while(1) {
      c = fgetc(fp);
      if( feof(fp) ) { 
         break ;
      }

      printf("%c", c);
   }
   fclose(fp);

}

void test_fill(void){
    int i = 0;
	for (i=0;i<10;i++){
        loader.points[i] = newvec3( 2.0, 1.0, 9.0 );
	}
}

void show_object(void)
{
    int i = 0;
	for (i=0;i<10;i++){
      print_vec3( loader.points[i]) ;
        //printf("pt is %s %s %s \n", pt.x, pt.y, pt.z );
	}
}


void save_objfile( char *filepath)
{

   FILE * fp;

   fp = fopen (filepath, "w+");
   fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);
   
   fclose(fp);
      
}




