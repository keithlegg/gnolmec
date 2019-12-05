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

    struct vec3 points[1000];
    
    char *data;

};

void test_fill(void);
void show_object(void);

void load_objfile( char *filepath);
void save_objfile( char *filepath);




#endif
