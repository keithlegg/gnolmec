#ifndef OBJMODEL_H    
#define OBJMODEL_H

#include "math_op.h"


//dynamically set the size of the 3D object 
//DEBUG TODO - ENSURE OBJECT EXISTS IN HEAP, NOT STACK  

const int num_vtx   = 1000;
const int num_faces = 1000;



typedef struct line{
    int pt1;
    int pt2;
};

typedef struct triangle{
    int pt1;
    int pt2;
    int pt3;    
};

typedef struct quad{
    int pt1;
    int pt2;
    int pt3;    
    int pt4;
};

typedef struct obj_model{
    int num_pts;
    int num_uvs;

    int num_lines;
    int num_tris;
    int num_quads;    

    struct vec2 uvs[num_vtx];         // UV coords 
    struct vec3 points[num_vtx];      // 3 floats  
    //struct vec3 vtx_colors[1000];   

    struct line lines[num_faces];       //2 point lines 
    struct triangle tris[num_faces];    //3 point polygons 
    struct quad quads[num_faces];       //4 point polygons 

};


void reset_objfile(struct obj_model* loader);
void load_objfile( char *filepath, struct obj_model* loader);
void save_objfile( char *filepath);

void test_loader_data(struct obj_model* loader);
void show_loader(struct obj_model* loader);






#endif
