/*****************************************************************************/
    /* 
        obj_model.c
        
            Load and save 3D models from disk 


        Copyright (C) 2019 Keith Legg  - keithlegg23@gmail.com
    
    */
         

/*****************************************************************************/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "obj_model.h"


int uv_cnt   = 0;  // number of UVs loaded 
 





void get_obj_info(struct obj_model* loader, struct obj_info* obinfo)
{
    obinfo->bb_min_x = 0;
    obinfo->bb_max_x = 0;
    obinfo->bb_min_y = 0;
    obinfo->bb_max_y = 0;
    obinfo->bb_min_z = 0;
    obinfo->bb_max_z = 0; 

    int x = 0;
    vec3 point;

    for(x=0;x<loader->num_pts;x++)
    {
        
        point = loader->points[x];

        if (point.x < obinfo->bb_min_x){
            obinfo->bb_min_x = point.x;    
        }
        if (point.x > obinfo->bb_max_x){
            obinfo->bb_max_x = point.x;    
        }

        if (point.y < obinfo->bb_min_y){
            obinfo->bb_min_y = point.y;    
        }
        if (point.y > obinfo->bb_max_y){
            obinfo->bb_max_y = point.y;    
        }

        if (point.z < obinfo->bb_min_z){
            obinfo->bb_min_z = point.z;    
        }
        if (point.z > obinfo->bb_max_z){
            obinfo->bb_max_z = point.z;    
        }        
        
        // print_vec3(point);

    }

    // printf("minx %f maxx %f miny %f maxy %f minz %f maxz %f \n", obinfo->bb_min_x
    //                                                            , obinfo->bb_max_x
    //                                                            , obinfo->bb_min_y
    //                                                            , obinfo->bb_max_y
    //                                                            , obinfo->bb_min_z
    //                                                            , obinfo->bb_max_z );

}

/*******************************************************************/
void insert_geom(struct obj_model* from_obj, struct obj_model* to_obj)
{

    int x = 0;

    int pt_idx    = 0;
    int tri_idx   = 0;
    int quad_idx  = 0;
    int line_idx  = 0;

    //printf("## ##  %d %d \n", from_obj->num_pts, from_obj->num_tris);

    //points
    for (x=0;x<from_obj->num_pts;x++)
    {
        to_obj->points[pt_idx] = from_obj->points[x];
        pt_idx++; 
    }

    //triangles
    for (x=0;x<from_obj->num_tris;x++)
    {
        to_obj->tris[tri_idx] = from_obj->tris[x];
        tri_idx++; 
    }

    //quads 
    for (x=0;x<from_obj->num_quads;x++)
    {
        to_obj->quads[quad_idx] = from_obj->quads[x];
        quad_idx++; 
    }

    //lines
    for (x=0;x<from_obj->num_lines;x++)
    {
        to_obj->lines[line_idx] = from_obj->lines[x];
        line_idx++; 
    }

    //normals 

    //UVs 


    to_obj->num_pts = pt_idx;
    //to_obj->num_uvs = 0;
    to_obj->num_lines = line_idx;
    to_obj->num_tris = tri_idx;
    to_obj->num_quads = quad_idx;  
   

    //printf("# %d %d \n", to_obj->num_pts, to_obj->num_tris);

}


/*******************************************************************/
void reset_objfile(struct obj_model* loader, struct obj_info* obinfo)
{

    // uv_cnt   = 0;  // number UVs loaded 
    

    loader->num_pts = 0;
    loader->num_uvs = 0;
    loader->num_lines = 0;
    loader->num_tris = 0;
    loader->num_quads = 0;    

    obinfo->bb_min_x = 0;
    obinfo->bb_max_x = 0;
    obinfo->bb_min_y = 0;
    obinfo->bb_max_y = 0;
    obinfo->bb_min_z = 0;
    obinfo->bb_max_z = 0; 

    // loader->uvs[num_vtx];      // UV coords 
    // loader->points[num_vtx];   // 3 floats  
    // loader->lines[num_faces];  //2 point lines 
    // loader->tris[num_faces];   //3 point polygons 
    // loader->quads[num_faces];  //4 point polygons 

}

/*******************************************************************/

void show_loader(struct obj_model* loader)
{
    int i = 0;
  for (i=0;i<10;i++){
      print_vec3( loader->points[i]) ;
        //printf("pt is %s %s %s \n", pt.x, pt.y, pt.z );
  }
}

/*******************************************************************/

void load_objfile( char *filepath, struct obj_model* loader)
{
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filepath, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    // walk the file line by line
    while ((read = getline(&line, &len, fp)) != -1) {

        char coords_str[256]; // string that verts get copied to 
        char fidx_str[256];   // string that faces get copied to

        // printf("%s",line);

        // walk the line, token by token  
        char* tok_spacs = strtok(line, " ");
        while (tok_spacs) 
        {
              
            /******************************/
            // look for V / vertices
            if ( strcmp( tok_spacs, "v") == 0)
            {
                strcpy (coords_str, tok_spacs+2);

                // printf("%s\n", coords_str); // <- vertex line 

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(coords_str, " ");
                int vidx = 0;
                
                float xc, yc, zc = 0.0;

                while (tok_line) 
                {
                    // printf("%s \n", tok_line );   
                    
                    if(vidx==0){
                        xc = atof(tok_line);
                    }
                    if(vidx==1){
                        yc = atof(tok_line);                        
                    }  
                    if(vidx==2){
                        zc = atof(tok_line);
                    }                                        
                    
                    vidx++;tok_line = strtok(NULL, " ");
                }
                
                //if two points its a 2D coord (probably not a standard obj file )  
                if (vidx==2){
                    printf("2D point detected! \n"); 
                }
                
                //if three points its a proper vertex 
                if (vidx==3){
                    vec3 vpt = newvec3( xc, yc, zc  );
                    loader->points[loader->num_pts] = vpt;
                    loader->num_pts++;
                    // print_vec3(vpt); //to view output 
                }                


            }//end vertex loader 
            
            /******************************/

            //  look for F / faces
            if ( strcmp( tok_spacs, "f") == 0)
            {

                strcpy (fidx_str, tok_spacs+2);
                char* tok_line = strtok(fidx_str, " ");
                int fidx = 0;
                
                int pt1,pt2,pt3,pt4 = 0;;

                // walk the tokens on the line 
                // ASSUME TRIANGLES ONLY! (3 coords per vertex)
                while (tok_line) 
                {
                    //printf("%d %s\n", fidx, tok_line); // <- face line                  

                    //only supports 2,3,4 sided polygons  
                    if(fidx==0){
                        pt1 = atoi( tok_line);
                    }
                    if(fidx==1){
                        pt2 = atoi( tok_line);                       
                    }  
                    if(fidx==2){
                        pt3 = atoi( tok_line);
                    }   
                    if(fidx==3){
                        pt4 = atoi( tok_line);                       
                    }  
                    /***********/

                    //n = atoi (buffer);
                    fidx++;tok_line = strtok(NULL, " ");

                }

                /***********/
                //TODO - implement single point visualization !
                
                /***********/                    
                //if two face indices - its a line  
                if (fidx==2){
                    loader->lines[loader->num_lines].pt1 = pt1;
                    loader->lines[loader->num_lines].pt2 = pt2;                          
                    loader->num_lines++;                    
                }

                if (fidx==3){

                    // if you want the actual point data from this index
                    // print_vec3(loader->points[pt1]);

                    //or just store the indices
                    loader->tris[loader->num_tris].pt1 = pt1;
                    loader->tris[loader->num_tris].pt2 = pt2;                          
                    loader->tris[loader->num_tris].pt3 = pt3;

                    loader->num_tris++;

                }

                if (fidx==4){
                    loader->quads[loader->num_quads].pt1 = pt1;
                    loader->quads[loader->num_quads].pt2 = pt2;                          
                    loader->quads[loader->num_quads].pt3 = pt3;
                    loader->quads[loader->num_quads].pt4 = pt4;
                    loader->num_quads++;
                }

            }//end face loader

            /******************************/

            //  look for UV coordinates
            if ( strcmp( tok_spacs, "vt") == 0)
            {
               // uv_cnt++;
            }


            /******************************/
            tok_spacs = strtok(NULL, " ");

        }

    }

    fclose(fp);
    if (line)
        free(line);

    // ---------------------------------------------
    loader->num_uvs = 0;


    printf("\n\n---------------------------\n"  ) ;
    printf("%d vertices loaded   \n", loader->num_pts    ) ;
    printf("%d uvs loaded        \n", loader->num_uvs    ) ; 
    printf("%d lines loaded      \n", loader->num_lines  ) ;
    printf("%d triangles loaded  \n", loader->num_tris   ) ;
    printf("%d quads loaded      \n", loader->num_quads  ) ;    
}




/*******************************************************************/

void test_loader_data(struct obj_model* loader){

    loader->uvs[0]    = newvec2(  0.0 , 0.0         );  
    loader->points[0] = newvec3( -1.0 , -1.0,  1.0  );
    loader->uvs[1]    = newvec2( 1.0  , 0.0         );  
    loader->points[1] = newvec3( 1.0 , -1.0,  1.0   );
    loader->uvs[2]    = newvec2( 1.0, 1.0           );  
    loader->points[2] = newvec3( 1.0,  1.0,  1.0    );

    // Back Face
    loader->uvs[3]     = newvec2( 1.0, 0.0           );  
    loader->points[3]  = newvec3( -1.0, -1.0, -1.0   );
    loader->uvs[4]     = newvec2( 1.0, 1.0           );  
    loader->points[4]  = newvec3( -1.0,  1.0, -1.0   );
    loader->uvs[5]     = newvec2( 0.0, 1.0           );  
    loader->points[5]  = newvec3( 1.0,  1.0, -1.0    );

    //  uvs     // UV coords 
    //  points  // 3 floats  
    //  faces   // triangles only 
                    
}





/*******************************************************************/


void save_objfile( char *filepath)
{

   FILE * fp;

   fp = fopen (filepath, "w+");
   fprintf(fp, "%s %s %s %d", "i", "an", "not working yet", 555);
   
   fclose(fp);
      
}




