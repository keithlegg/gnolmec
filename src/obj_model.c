/*****************************************************************************/
    /* 
        obj_model.c
        
            Load and save 3D models from disk 


        Copyright (C) 2019 Keith Legg  (keithlegg23@gmail.com)
    
    */
         

/*****************************************************************************/



#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "obj_model.h"




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

    int vtx_cnt = 0;  // number of verts loaded 
    int face_cnt = 0; // number of faces loaded 
    int uv_cnt  = 0;  // number of UVs loaded 

    // walk the file line by line
    while ((read = getline(&line, &len, fp)) != -1) {
     
        // printf("Retrieved line of length %zu:\n", read);
        char coords_str[256];
        char fidx_str[256];

        // walk the line, token by token  
        char* tok_spacs = strtok(line, " ");
        while (tok_spacs) 
        {
              
            // look for V / vertices
            if ( strcmp( tok_spacs, "v") == 0)
            {
                strcpy (coords_str, tok_spacs+2);

                // printf("%s\n", coords_str); // <- vertex line 

                //walk the tokens on the line (a copy of it)
                char* tok_line = strtok(coords_str, " ");
                int idx = 0;
                
                float xc, yc, zc = 0.0;

                while (tok_line) 
                {
                    // printf("%s \n", tok_line );   
                    if(idx==0){
                        xc = atof(tok_line);
                    }
                    if(idx==1){
                        yc = atof(tok_line);                        
                    }  
                    if(idx==2){
                        zc = atof(tok_line);
                        vec3 vpt = newvec3( xc, yc, zc  );
                        loader->points[vtx_cnt] = vpt;
                        vtx_cnt++;
                        // print_vec3(vpt); //to view output 
                    }                                        
                    
                    idx++;tok_line = strtok(NULL, " ");
                }

            }//end vertex loader 
            
            /************/

            //  look for F / faces
            if ( strcmp( tok_spacs, "f") == 0)
            {
               

                strcpy (fidx_str, tok_spacs+2);
                char* tok_line = strtok(fidx_str, " ");
                int idx = 0;
                
                int pt1,pt2,pt3,pt4 = 0;;

                // walk the tokens on the line 
                // ASSUME TRIANGLES ONLY! (3 coords per vertex)
                while (tok_line) 
                {
                    //printf("%d %s\n", idx, tok_line); // <- face line                  

                    if(idx==0){
                        pt1 = atoi( tok_line);
                    }
                    if(idx==1){
                        pt2 = atoi( tok_line);                       
                    }  
                    if(idx==2){
                        pt3 = atoi( tok_line);

                        // if you want the actual polygons
                        // print_vec3(loader->points[pt1]);
                        // print_vec3(loader->points[pt2]);
                        // print_vec3(loader->points[pt3]);

                        //or just store the indices
                        loader->tris[face_cnt].pt1 = pt1;
                        loader->tris[face_cnt].pt2 = pt2;                          
                        loader->tris[face_cnt].pt3 = pt3;

                        face_cnt++;

                    }   

                    //4 sided not implemented yet 
                    // if(idx==3){
                    //     printf(" idx 3:%s",tok_line);
                    //     //z = atoi (buffer);                        
                    // }  

                    //n = atoi (buffer);
                    idx++;tok_line = strtok(NULL, " ");

                }

            }//end face loader

            /************/

            //  look for UV coordinates
            if ( strcmp( tok_spacs, "vt") == 0)
            {

            }

            /************/   

            // loader->uvs[5]     = newvec2( 0.0, 1.0       ); // Top Left Of The Texture and Quad
            // loader->points[i]  = newvec3( 2.0, 1.0, 9.0  );


            tok_spacs = strtok(NULL, " ");

        }

    }

    fclose(fp);
    if (line)
        free(line);

    loader->num_pts = vtx_cnt;
    loader->num_faces = face_cnt;
    loader->num_uvs = 0;

    printf("\n\n---------------------------\n", vtx_cnt ) ;
    printf("%d vertices loaded \n", vtx_cnt ) ;
    printf("%d faces loaded    \n", face_cnt ) ;

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

void show_loader(struct obj_model* loader)
{
    int i = 0;
  for (i=0;i<10;i++){
      print_vec3( loader->points[i]) ;
        //printf("pt is %s %s %s \n", pt.x, pt.y, pt.z );
  }
}


/*******************************************************************/


void save_objfile( char *filepath)
{

   FILE * fp;

   fp = fopen (filepath, "w+");
   fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);
   
   fclose(fp);
      
}




