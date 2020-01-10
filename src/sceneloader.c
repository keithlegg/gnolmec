
/*************************************************************/
/*
   sceneloader.c 

   Read scene.olm for a proto-dream scene format  

   Copyright (C) 2019 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/

#include <vector>
#include <iostream>
using namespace std;

// #include <stdlib.h>
// #include <stdio.h>

#include <string.h>
// #include <unistd.h>      
// #include <cmath>

//#include "gl_setup.h"
//#include "bitmap_io.h" // includes framebuffer.h, colors, etc 
//#include "point_op.h"
//#include "obj_model.h"
//#include "gnolmec.h"

#include "math_op.h"
#include "sceneloader.h"

extern float cam_posx; 
extern float cam_posy;
extern float cam_posz;


 

//char obj_filepaths[100][100];
vector<string> obj_filepaths;
int num_loaded_obj = 0;

vector<vec3> scene_drawvec3;
vector<vec3> scene_drawvecclr;
int num_drawvec3 = 0;

vector<vec3> scene_drawpoints;
int num_drawpoints = 0;

/*

  filepath 
  material 
  matrix 
  pos 
  rotation 


*/

void clear_scenegeom( void)
{

    scene_drawvec3.clear();
    scene_drawvecclr.clear();
    num_drawvec3 = 0;

}

void read_scenefile( char* filepath )
{
    printf("#loading setup file %s \n", filepath );
    
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filepath, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    obj_filepaths.clear();
    float vx,vy,vz,cr,cg,cb = 0;

    // walk the file line by line
    while ((read = getline(&line, &len, fp)) != -1) 
    {

        char cmd_str[256]; // string that verts get copied to 
        
        // walk the line, token by token  
        char* tok_spacs = strtok(line, " ");
        while (tok_spacs) 
        {

            //-------------------------------             
            if ( strcmp( tok_spacs, "op_loadobj") == 0)
            {
                strcpy (cmd_str, line);

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(0, " ");
                
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0)
                    {
                        cout << "object found " << tok_line << "\n";
                        //strcpy(obj_filepaths[objct], tok_line);  
                        obj_filepaths.push_back(tok_line);
                        num_loaded_obj++;
                    }

                    tidx++;                                        
                    tok_line = strtok(NULL, " ");

                }
            }
            //-------------------------------
            if ( strcmp( tok_spacs, "op_draw_vec3") == 0)
            {
                strcpy (cmd_str, line);

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(0, " ");
                
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0){vx = atof(tok_line);}
                    if(tidx==1){vy = atof(tok_line);}
                    if(tidx==2){vz = atof(tok_line);}
                    if(tidx==3){cr = atoi(tok_line);}
                    if(tidx==4){cg = atoi(tok_line);}
                    if(tidx==5)                                                           
                    {
                        cb = atoi(tok_line); 
                        cout << "draw vec3 found " << vx <<" "<< vy << " "<< vz <<" RGB  " << cr <<" "<< cg <<" "<< cb <<"\n";
                        scene_drawvec3.push_back(newvec3(vx,vy,vz));
                        scene_drawvecclr.push_back(newvec3(cr,cg,cb)); 
                        num_drawvec3++;                       
                    }

                    tidx++;                                        
                    tok_line = strtok(NULL, " ");

                }
            }

            //-------------------------------
            if ( strcmp( tok_spacs, "op_draw_point") == 0)
            {
                strcpy (cmd_str, line);

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(0, " ");
                
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0)
                    {
                        cout << "draw point found " << tok_line << "\n";
                    }

                    tidx++;                                        
                    tok_line = strtok(NULL, " ");

                }
            }
           
      
            //-------------------------------
            tok_spacs = strtok(NULL, " ");
        }

    }

    //  
    //strcpy(obj_filepaths[1], "3d_obj/sphere.obj"); 


}

/*****************************************************

/*
   DEBUG - I added the exta "#" at the end because my c++ code is dumb
*/

void write_scenefile(char*objpath, char*cammatrixpath, char* scenefilepath )
{

    //char *object_path ="3d_obj/pycore.obj";
    //char *camera_matrix_path = " ";

    

    FILE * fp;

    fp = fopen (scenefilepath, "w+");


    fprintf(fp, "## Generated with Gnolmec.  ##\n\n"              );

    fprintf(fp, "obj_path %s #\n"            , objpath            );
    fprintf(fp, "cam_matrix_path %s #\n"     , cammatrixpath      );
    
    fprintf(fp, "cam_pos %f %f %f #\n"       , cam_posx, cam_posy, cam_posz          );

    fprintf(fp, "op_loadobj %s #\n"          , objpath            );



    fprintf(fp, "\n# light setup #\n"                             );
    fprintf(fp, "light_pos %s #\n"           , "0 5 0"            );
    fprintf(fp, "light_intensity %s #\n"     , "5.0"              );

    fprintf(fp, "\n# colors      #\n"                             );
    fprintf(fp, "bg_color %s #\n"            , "20 15 15"         ); 
    fprintf(fp, "line_color %s #\n"          , "0 0 100"          ); 
    fprintf(fp, "fill_color %s #\n"          , "105 105 105"      ); 
    fprintf(fp, "vtx_color %s #\n"           , "200 0 0"          ); 

    fprintf(fp, "show_vtx %s #\n"            , "false"            ); 
    fprintf(fp, "show_lines %s #\n"          , "false"            ); 

    fprintf(fp, "\n# render prefs    #\n"                         );
    fprintf(fp, "rendermode %s #\n"          , "litshaded"        ); 
 
    // 


    fprintf(fp, "\n\n## Commands to play with someday ##\n\n"     );
    //fprintf(fp, "#op_loadobj #\n"                                 );
   

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



