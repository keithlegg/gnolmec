
/*************************************************************/
/*
   sceneloader.c 

   Read setup.olm for a proto-dream scene format  

   Copyright (C) 2019 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/

#include <stdlib.h>
#include <stdio.h>
// #include <string.h>
// #include <unistd.h>      
// #include <cmath>

//#include "gl_setup.h"
//#include "bitmap_io.h" // includes framebuffer.h, colors, etc 
//#include "point_op.h"
//#include "obj_model.h"
//#include "gnolmec.h"

#include "math_op.h"
#include "sceneloader.h"



/*
   DEBUG - I added the exta "#" at the end because my c++ code is dumb
*/

void write_scenefile(char*objpath, char*cammatrixpath, char* scenefilepath )
{

    //char *object_path ="3d_obj/pycore.obj";
    //char *camera_matrix_path = " ";

    

    FILE * fp;

    fp = fopen (scenefilepath, "w+");


    fprintf(fp, "## Generated with Gnolmec.  ##\n\n"             );
    fprintf(fp, "obj_path %s #\n"            , objpath           );
    fprintf(fp, "cam_matrix_path %s #\n"     , cammatrixpath     );

    fprintf(fp, "\n# light setup #\n"                             );
    fprintf(fp, "light_pos %s #\n"           , "0 5 0"           );
    fprintf(fp, "light_intensity %s #\n"     , "1.8"              );

    fprintf(fp, "\n# colors      #\n"                              );
    fprintf(fp, "bg_color %s #\n"            , "20 15 15"        ); 
    fprintf(fp, "line_color %s #\n"          , "0 0 100"         ); 
    fprintf(fp, "fill_color %s #\n"          , "105 105 105"      ); 
    //fprintf(fp, "vtx_color %s #\n"           , "litshaded"       ); 

    fprintf(fp, "\n# render prefs    #\n"                         );
    fprintf(fp, "rendermode %s #\n"          , "litshaded"       ); 


    fprintf(fp, "\n\n## Commands to play with someday ##\n\n" );
    fprintf(fp, "#op_loadobj #\n" );
    fprintf(fp, "#op_loadobj #\n" );
    fprintf(fp, "#op_loadobj #\n" );    


    fclose(fp);
      
 


    /*
      "3dobj_path" 
      "bitmap_path"
   
      pos 
      rot

      ###################
      bg color 
      grid color 
      grid_disp true 
      line_disp true 
      tri_disp true 
      point_disp true 
      #this is a comment 
    */

};

/***************************************************************/


// load_matrix44()

void save_matrix44(char* filepath, m44 *input ){

   FILE * fp;

   fp = fopen (filepath, "w+");
   
   //---------------------- 
   //fprintf(fp, "%s %s %s %d", "i", "an", "not working yet", 555);
   
   fprintf(fp, "%f %f %f %f\n", input->m0  , input->m1  , input->m2  , input->m3  );
   fprintf(fp, "%f %f %f %f\n", input->m4  , input->m5  , input->m6  , input->m7  );
   fprintf(fp, "%f %f %f %f\n", input->m8  , input->m9  , input->m10 , input->m11 );
   fprintf(fp, "%f %f %f %f\n", input->m12 , input->m13 , input->m14 , input->m15 );      
   
   //----------------------
   fclose(fp);
      
}








/*
    scene_object
    load_scene()
    save_scene()
*/



