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




void test_loader_data(struct obj_model* loader){


    // glTexCoord2f(1.0f, 1.0f); glVertex3f( );  

    // // Back Face
    // glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
    // glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    // glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad

    // // Top Face
    // glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);  // Top Left Of The Texture and Quad
    // glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Bottom Left Of The Texture and Quad
    // glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Bottom Right Of The Texture and Quad

    // // Bottom Face       
    // glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Top Right Of The Texture and Quad
    // glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Top Left Of The Texture and Quad
    // glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // Bottom Left Of The Texture and Quad

    // // Right face
    // glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);  // Bottom Right Of The Texture and Quad
    // glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);  // Top Right Of The Texture and Quad
    // glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // Top Left Of The Texture and Quad

    // // Left Face
    // glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);  // Bottom Left Of The Texture and Quad
    // glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // Bottom Right Of The Texture and Quad
    // glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // Top Right Of The Texture and Quad


    //  uvs     // UV coords 
    //  points  // 3 floats  
    //  faces   // triangles only 

    loader->uvs[0]    = newvec2(  0.0 , 0.0          ); // Bottom Left Of The Texture and Quad
    loader->points[0] = newvec3( -1.0 , -1.0,  1.0 );

    loader->uvs[1]    = newvec2( 1.0  , 0.0          );// Bottom Right Of The Texture and Quad
    loader->points[1] = newvec3( 1.0 , -1.0,  1.0 );

    loader->uvs[2]    = newvec2( 1.0, 1.0      );      // Top Right Of The Texture and Quad
    loader->points[2] = newvec3( 1.0,  1.0,  1.0 );
    
    // loader.uvs[3]    = newvec2( 0.0, 0.0      );
    // loader.points[3] = newvec3( 2.0, 1.0, 9.0 );
    // loader.uvs[4]    = newvec2( 0.0, 0.0      );
    // loader.points[4] = newvec3( 2.0, 1.0, 9.0 );
    // loader.uvs[5]    = newvec2( 0.0, 0.0      );
    // loader.points[5] = newvec3( 2.0, 1.0, 9.0 );

            
                    
}

/*********************************/

void show_loader(struct obj_model* loader)
{
    int i = 0;
  for (i=0;i<10;i++){
      print_vec3( loader->points[i]) ;
        //printf("pt is %s %s %s \n", pt.x, pt.y, pt.z );
  }
}

/*********************************/

void load_objfile( char *filepath)
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
        // printf("Retrieved line of length %zu:\n", read);
        // printf("%s", line);

        // walk the line, token by token  
        char* token = strtok(line, " ");
        while (token) {
            printf("token: %s\n", token);
            token = strtok(NULL, " ");

            // loader.points[i] = newvec3( 2.0, 1.0, 9.0 );

        }

    }

    fclose(fp);
    if (line)
        free(line);
    //exit(EXIT_SUCCESS);
}



/*

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
      //////////
      // char s[256];
      //strcpy(s, c);
      printf("%c ",c );


      // char* token = strtok(s, " ");
      // while (token) {
      //     printf("token: %s\n", token);
      //     token = strtok(NULL, " ");
      // }
      //////////
      printf("%c", c);
   }
   fclose(fp);

}
*/




void save_objfile( char *filepath)
{

   FILE * fp;

   fp = fopen (filepath, "w+");
   fprintf(fp, "%s %s %s %d", "We", "are", "in", 2012);
   
   fclose(fp);
      
}




