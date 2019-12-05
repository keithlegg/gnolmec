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




void load_objfile( char *filepath)
{

   FILE *fp;
   int c;
  
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





void save_objfile( char *filepath)
{

   FILE * fp;

   fp = fopen (filepath, "w+");
   fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);
   
   fclose(fp);
      
}




