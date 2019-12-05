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

    unsigned int sizeX;
    unsigned int sizeY;
    
    char *data;

};



void load_objfile( char *filepath);
void save_objfile( char *filepath);




#endif
