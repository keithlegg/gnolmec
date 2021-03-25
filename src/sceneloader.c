
/*************************************************************/
/*
   sceneloader.c 

   Read scene.olm for a proto-dream scene format  

   Copyright (C) 2019 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/

#include <vector>
#include <iostream>
#include <string.h>
#include <fstream>

#include <algorithm> // for std::find

#include "obj_model.h"
#include "gl_setup.h"

// USE MAKEFILE TO SET THIS 
#ifdef _ISLINUX
    #include <bits/stdc++.h> 
#else
    #include "bits_stdc++.h" 
#endif






//#include "gl_setup.h"
//#include "bitmap_io.h" // includes framebuffer.h, colors, etc 
//#include "point_op.h"
//#include "obj_model.h"
//#include "gnolmec.h"

#include "math_op.h"
#include "sceneloader.h"


using namespace std;


extern float cam_posx; 
extern float cam_posy;
extern float cam_posz;

extern float light_posx; 
extern float light_posy;
extern float light_posz;

extern int surfce_clr_r; 
extern int surfce_clr_g; 
extern int surfce_clr_b; 

extern int line_clr_r; 
extern int line_clr_g; 
extern int line_clr_b; 

extern double light_intensity; 

extern bool draw_points;   
extern bool draw_lines;  
extern bool draw_normals;  
extern bool draw_quads; 
extern bool draw_triangles; 

extern bool draw_grid;
extern bool draw_cntrgrid;

extern float gridsize;
extern float gnomonsize;

extern GLfloat clr_linez[];


char strbuffer[100][100];
vector<string> obj_filepaths;
int num_loaded_obj = 0;

vector<vec3> scene_drawvec3;
vector<vec3> scene_drawvecclr;
int num_drawvec3 = 0;

vector<vec3> scene_drawpoints;
vector<vec3> scene_drawpointsclr;
vector<vec3>* pt_scene_drawpoints = &scene_drawpoints;
int num_drawpoints = 0;


const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;

extern obj_model pt_model_buffer;


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

/*****************************************************/


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
            if ( strcmp( tok_spacs, "drawlines") == 0)
            {
                strcpy (cmd_str, line);

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(NULL, " \t\n");
                
                int tidx = 0;
                while (tok_line) 
                {
                    char* check_token = strtok(NULL, " \t\n");
                    if(strcmp( tok_line, "true")==0)
                    {
                        draw_lines = true;
                    }else{
                        draw_lines = false;                        
                        printf("# draw lines %s\n", draw_lines ? "true" : "false");
                    }
                                                          
                    tok_line = strtok(NULL, " \t\n");

                }
          
            }


            //-------------------------------
            if ( strcmp( tok_spacs, "gridsize") == 0)
            {
                strcpy (cmd_str, line);
                char* tok_line = strtok(0, " ");
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0){gridsize = atof(tok_line);}
                    tidx++;                                        
                    tok_line = strtok(NULL, " \t\n");                    
                }
            }

            //-------------------------------
            if ( strcmp( tok_spacs, "gnomonsize") == 0)
            {
                strcpy (cmd_str, line);
                char* tok_line = strtok(0, " ");
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0){gnomonsize = atof(tok_line);}
                    tidx++;                                        
                    tok_line = strtok(NULL, " \t\n");                    
                }
            }

            //-------------------------------             
            if ( strcmp( tok_spacs, "showgnomon") == 0)
            {
                strcpy (cmd_str, line);

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(NULL, " \t\n");
                
                int tidx = 0;
                while (tok_line) 
                {
                    char* check_token = strtok(NULL, " \t\n");
                    if(strcmp( tok_line, "true")==0)
                    {
                        draw_cntrgrid = true;
                    }
                    if(strcmp( tok_line, "false")==0){
                        draw_cntrgrid = false;                        
                        printf("# draw_cntrgrid %s\n", draw_cntrgrid ? "true" : "false");
                    }
                                                          
                    tok_line = strtok(NULL, " \t\n");

                }
          
            }
           
            //-------------------------------             
            if ( strcmp( tok_spacs, "showgrid") == 0)
            {
                strcpy (cmd_str, line);

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(NULL, " \t\n");
                
                int tidx = 0;
                while (tok_line) 
                {
                    char* check_token = strtok(NULL, " \t\n");
                    if(strcmp( tok_line, "true")==0)
                    {
                        draw_grid = true;
                    };
                    if(strcmp( tok_line, "false")==0){
                        draw_grid = false;                        
                        printf("# draw_grid %s\n", draw_grid ? "true" : "false");
                    }
                                                          
                    tok_line = strtok(NULL, " \t\n");

                }
          
            }

            //-------------------------------             
            if ( strcmp( tok_spacs, "drawquads") == 0)
            {
                strcpy (cmd_str, line);

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(NULL, " \t\n");
                
                int tidx = 0;
                while (tok_line) 
                {
                    char* check_token = strtok(NULL, " \t\n");
                    if(strcmp( tok_line, "true")==0)
                    {
                        draw_quads = true;
                    }else{
                        draw_quads = false;                        
                        printf("# draw quads %s\n", draw_quads ? "true" : "false");
                    }
                                                          
                    tok_line = strtok(NULL, " \t\n");

                }
          
            }

            //-------------------------------             
            if ( strcmp( tok_spacs, "drawtriangles") == 0)
            {
                strcpy (cmd_str, line);

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(NULL, " \t\n");
                
                int tidx = 0;
                while (tok_line) 
                {
                    char* check_token = strtok(NULL, " \t\n");
                    if(strcmp( tok_line, "true")==0)
                    {
                        draw_triangles = true;
                    }else{
                        draw_triangles = false;                        
                        printf("# draw triangles %s\n", draw_triangles ? "true" : "false");
                    }
                                                          
                    tok_line = strtok(NULL, " \t\n");

                }
          
            }

            //-------------------------------             
            //-------------------------------             
            if ( strcmp( tok_spacs, "drawpoints") == 0)
            {
                strcpy (cmd_str, line);

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(NULL, " \t\n");
                
                int tidx = 0;
                while (tok_line) 
                {
                    char* check_token = strtok(NULL, " \t\n");
                    if(strcmp( tok_line, "true")==0)
                    {
                        draw_points = true;
                    }else{
                        draw_points = false;                        
                        printf("# draw points %s\n", draw_points ? "true" : "false");
                    }
                                                     
                    tok_line = strtok(NULL, " \t\n");

                }
          
            }

            //-------------------------------             
            if ( strcmp( tok_spacs, "op_loadobj") == 0)
            {
                strcpy (cmd_str, line);
                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(NULL, " \t\n");
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0)
                    {
                        //cout << "#object found " << tok_line << "\n";
                        if (find(obj_filepaths.begin(), obj_filepaths.end(), tok_line) == obj_filepaths.end())
                        {
                            obj_filepaths.push_back(tok_line);
                            num_loaded_obj++;
                        }
                    }
                    tidx++;                                        
                    tok_line = strtok(NULL, " \t\n");
                }
            }
            //-------------------------------
            if ( strcmp( tok_spacs, "light_intensity") == 0)
            {
                strcpy (cmd_str, line);
                char* tok_line = strtok(0, " ");
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0){light_intensity = atof(tok_line);}
                    tidx++;                                        
                    tok_line = strtok(NULL, " \t\n");                    
                }
            }
            //-------------------------------
            if ( strcmp( tok_spacs, "light_pos") == 0)
            {
                strcpy (cmd_str, line);
                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(0, " ");
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0){light_posx = atof(tok_line);}
                    if(tidx==1){light_posy = atof(tok_line);}
                    if(tidx==2){light_posz = atof(tok_line);}
                    tidx++;                                        
                    tok_line = strtok(NULL, " ");
                }
            }
            //-------------------------------
            if ( strcmp( tok_spacs, "op_draw_pnt") == 0)
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
                        cout << "draw point found " << vx <<" "<< vy << " "<< vz <<" RGB  " << cr <<" "<< cg <<" "<< cb <<"\n";
                        scene_drawpoints.push_back(newvec3(vx,vy,vz));
                        scene_drawpointsclr.push_back(newvec3(cr,cg,cb)); 
                        num_drawpoints++;

                        //obj has no point geom type without polygon - should we do it that way?
                        //pt_model_buffer->num_pts;

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
            // //------------------------------
            // if ( strcmp( tok_spacs, "op_draw_point") == 0)
            // {
            //     strcpy (cmd_str, line);
            //     //walk the tokens on the line (a copy of it)
            //     char* tok_line = strtok(0, " ");
            //     int tidx = 0;
            //     while (tok_line) 
            //     {
            //         if(tidx==0)
            //         {
            //             cout << "draw point found " << tok_line << "\n";
            //         }
            //         tidx++;                                        
            //         tok_line = strtok(NULL, " ");
            //     }
            // }
            //------------------------------
            if ( strcmp( tok_spacs, "fill_color") == 0)
            {
                strcpy (cmd_str, line);
                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(0, " ");
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0){surfce_clr_r = atoi(tok_line);}
                    if(tidx==1){surfce_clr_g = atoi(tok_line);}
                    if(tidx==2){surfce_clr_b = atoi(tok_line);}
                    tidx++;                                        
                    tok_line = strtok(NULL, " ");
                }
            }

            //------------------------------
            if ( strcmp( tok_spacs, "line_color") == 0)
            {
                strcpy (cmd_str, line);
                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(0, " ");
                int tidx = 0;
                while (tok_line) 
                {
                    if(tidx==0){line_clr_r = atoi(tok_line);}
                    if(tidx==1){line_clr_g = atoi(tok_line);}
                    if(tidx==2){line_clr_b = atoi(tok_line);}
                    tidx++;                                        
                    tok_line = strtok(NULL, " ");
                }

                clr_linez[0] = line_clr_r;
                clr_linez[0] = line_clr_g;
                clr_linez[0] = line_clr_b;
                clr_linez[0] = 0;                                


            }

            //-------------------------------
            tok_spacs = strtok(NULL, " ");
        }
    }

    //strcpy(obj_filepaths[1], "3d_obj/sphere.obj"); 

}

/*****************************************************/

void write_scenefile(char*objpath, char*cammatrixpath, char* scenefilepath )
{

    //char *object_path ="3d_obj/pycore.obj";
    //char *camera_matrix_path = " ";

    FILE * fp;

    fp = fopen (scenefilepath, "w+");


    fprintf(fp, "## Generated with Gnolmec.  ##\n\n"             );

    fprintf(fp, "obj_path %s \n"            , objpath                                    );
    fprintf(fp, "cam_matrix_path %s \n"     , cammatrixpath                              );
    fprintf(fp, "cam_pos %f %f %f \n"       , cam_posx, cam_posy, cam_posz               );
    fprintf(fp, "op_loadobj %s \n"          , objpath                                    );

    fprintf(fp, "\n# render prefs   \n"                                                  );
    fprintf(fp, "rendermode %s \n"          , "litshaded"                                ); 
    fprintf(fp, "showgrid true       \n"                                                   );
    fprintf(fp, "showgnomon false    \n"                                                   ); 
    fprintf(fp, "gridsize 2.5        \n"                                                   );
    fprintf(fp, "gnomonsize 1.0      \n"                                                   ); 
    fprintf(fp, "drawlines true      \n"                                                   );
    fprintf(fp, "drawquads true      \n"                                                   ); 
    fprintf(fp, "drawtriangles true  \n"                                                   );
    fprintf(fp, "drawpoints true      \n"                                                   ); 

    fprintf(fp, "\n# light setup \n"                                                     );
    fprintf(fp, "light_pos %f %f %f \n"     , light_posx, light_posy, light_posz         );
    fprintf(fp, "light_intensity %f \n"     , light_intensity                            );

    fprintf(fp, "\n# colors      \n"                                                     );
    fprintf(fp, "bg_color %s \n"            , "20 15 15"                                 ); 
    fprintf(fp, "line_color %s \n"          , "0 0 100"                                  ); 
    fprintf(fp, "fill_color %d %d %d \n"    , surfce_clr_r, surfce_clr_g, surfce_clr_b   ); 
    fprintf(fp, "vtx_color %s \n"           , "200 0 0"                                  ); 
    fprintf(fp, "show_vtx %s \n"            , "false"                                    ); 
    fprintf(fp, "show_lines %s \n"          , "false"                                    ); 
 
 
    // 


    //fprintf(fp, "\n\n## Commands to play with someday ##\n\n"     );
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



void load_matrix33(char* filename, m33 * pm33 )
{
    ifstream fin;

    fin.open(filename); // open a file
    if (!fin.good()){ 
        cout << "matrix text file \""<< filename <<"\" appears to be missing or broken." << endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    int l_idx = 0;

    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
         
        vector <string> tokens; 
        stringstream check1(buf); 
        string intermediate; 
        
        while(getline(check1, intermediate, ' ')) 
        { 
            tokens.push_back(intermediate); 
        } 
        
        int t_idx = 0; 
        for(int i = 0; i < tokens.size(); i++)
        { 

            // omit blank spaces 
            if ( tokens[i].compare(" ") != -1)
            {   
                double elem = stod(tokens[i]);

                if (l_idx==0 && t_idx==0 )
                    pm33->m0 = elem;
                if (l_idx==0 && t_idx==1 )
                    pm33->m1 = elem;
                if (l_idx==0 && t_idx==2 )
                    pm33->m2 = elem;
                                                      

                if (l_idx==1 && t_idx==0 )
                    pm33->m3 = elem;
                if (l_idx==1 && t_idx==1 )
                    pm33->m4 = elem;
                if (l_idx==1 && t_idx==2 )
                    pm33->m5 = elem;
 

                if (l_idx==2 && t_idx==0 )
                    pm33->m6 = elem;
                if (l_idx==2 && t_idx==1 )
                    pm33->m7 = elem;
                if (l_idx==2 && t_idx==2 )
                    pm33->m8 = elem;
      
                t_idx++; // non space token index

            }

        }
       
        l_idx++; //line index
  
   }
      
}

/***************************************************************/

void load_matrix44(char* filename, m44 * pm44 )
{
    ifstream fin;

    fin.open(filename); // open a file
    if (!fin.good()){ 
        cout << "matrix text file \""<< filename <<"\" appears to be missing or broken." << endl;
        exit (EXIT_FAILURE); // exit if file not found
    }

    int l_idx = 0;

    while (!fin.eof())
    {
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
         
        vector <string> tokens; 
        stringstream check1(buf); 
        string intermediate; 
        
        while(getline(check1, intermediate, ' ')) 
        { 
            tokens.push_back(intermediate); 
        } 
        
        int t_idx = 0; 
        for(int i = 0; i < tokens.size(); i++)
        { 

            // omit blank spaces 
            if ( tokens[i].compare(" ") != -1)
            {   
                double elem = stod(tokens[i]);

                if (l_idx==0 && t_idx==0 )
                    pm44->m0 = elem;
                if (l_idx==0 && t_idx==1 )
                    pm44->m1 = elem;
                if (l_idx==0 && t_idx==2 )
                    pm44->m2 = elem;
                if (l_idx==0 && t_idx==3 )
                    pm44->m3 = elem;                                                        

                if (l_idx==1 && t_idx==0 )
                    pm44->m4 = elem;
                if (l_idx==1 && t_idx==1 )
                    pm44->m5 = elem;
                if (l_idx==1 && t_idx==2 )
                    pm44->m6 = elem;
                if (l_idx==1 && t_idx==3 )
                    pm44->m7 = elem;  

                if (l_idx==2 && t_idx==0 )
                    pm44->m8 = elem;
                if (l_idx==2 && t_idx==1 )
                    pm44->m9 = elem;
                if (l_idx==2 && t_idx==2 )
                    pm44->m10 = elem;
                if (l_idx==2 && t_idx==3 )
                    pm44->m11 = elem;  

                if (l_idx==3 && t_idx==0 )
                    pm44->m12 = elem;
                if (l_idx==3 && t_idx==1 )
                    pm44->m13 = elem;
                if (l_idx==3 && t_idx==2 )
                    pm44->m14 = elem;
                if (l_idx==3 && t_idx==3 )
                    pm44->m15 = elem;                                      


                t_idx++; // non space token index

            }

        }
       
        l_idx++; //line index
  
   }
      
}


/***************************************************************/
void save_matrix44(char* filepath, m44 *input ){

   FILE * fp;

   fp = fopen (filepath, "w+");
   
   fprintf(fp, "%f %f %f %f\n", input->m0  , input->m1  , input->m2  , input->m3  );
   fprintf(fp, "%f %f %f %f\n", input->m4  , input->m5  , input->m6  , input->m7  );
   fprintf(fp, "%f %f %f %f\n", input->m8  , input->m9  , input->m10 , input->m11 );
   fprintf(fp, "%f %f %f %f\n", input->m12 , input->m13 , input->m14 , input->m15 );      
   
   fclose(fp);
      
}

/***************************************************************/

void save_matrix33(char* filepath, m33 *input ){

   FILE * fp;

   fp = fopen (filepath, "w+");
 
   fprintf(fp, "%f %f %f \n", input->m0  , input->m1  , input->m2   );
   fprintf(fp, "%f %f %f \n", input->m3  , input->m4  , input->m5   );
   fprintf(fp, "%f %f %f \n", input->m6  , input->m7  , input->m8   );
      
   fclose(fp);
      
}



