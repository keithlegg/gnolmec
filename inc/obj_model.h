#ifndef OBJMODEL_H    
#define OBJMODEL_H

#include "math_op.h"


// from math_op  
// struct vec3 {
//     double x;
//     double y;
//     double z;    
// };


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
    int num_faces;
    int num_uvs;

    struct vec2 uvs[1000];     // UV coords 
    struct vec3 points[1000];  // 3 floats  
    
    //struct vec3 faces[1000];  // 3 floats (ints) for triangles only  
    struct triangle tris[1000]; //3 face polygons 
    struct quad quads[1000];    //4 face polygons 

};


void test_loader_data(struct obj_model* loader);
void show_loader(struct obj_model* loader);


void load_objfile( char *filepath, struct obj_model* loader);
void save_objfile( char *filepath);




#endif
