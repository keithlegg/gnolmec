#ifndef OBJMODEL_H    
#define OBJMODEL_H

#include "math_op.h"


// from math_op  
// struct vec3 {
//     double x;
//     double y;
//     double z;    
// };




typedef struct obj_model{

    struct vec2 uvs[1000]; // UV coords 
    struct vec3 points[1000];     // 3 floats  
    struct vec3 faces[1000];      // 3 floats (ints) for triangles only  

    //char *data;

};


void test_loader_data(struct obj_model* loader);
void show_loader(struct obj_model* loader);


void load_objfile( char *filepath);
void save_objfile( char *filepath);




#endif
