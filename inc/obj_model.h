#ifndef OBJMODEL_H    
#define OBJMODEL_H

#include "math_op.h"




//DEBUG TODO - dynamically set the size of the 3D object 
// get this off the stack and into heap. Not sure how to go about it
// the problem is you cant easily do a struct of structs 
// OPTION1 - flatten it into a primitive singular struct


//OPTION2 - make a constructor/destructor that mallocs all the constituent structs
//then returns an array of pointers to thos, then allowing you to free later.... 

// void *ary[10];
// ary[0] = new int();
// ary[1] = new float();

// OPTION3 - this is not an option - you only get 1 dynamic type per struct 
// struct obj_model loader;
// struct vec2 uvs[50];
// loader.uvs = uvs;


//For now we work on the stack. It will die if you load a large model 
const int num_vtx   = 12000;
const int num_faces = 5000;


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


typedef struct obj_info{
    int num_pts;
    int num_uvs;

    int num_tris;
    int num_faces;
    int num_quads;

    float centroid_x;
    float centroid_y;
    float centroid_z;

    float bb_min_x;
    float bb_max_x;
    
    float bb_min_y;
    float bb_max_y;

    float bb_min_z;
    float bb_max_z;    

};

vec3 get_extents(struct obj_info* obinfo);
vec3 get_centroid(struct obj_info* obinfo);

void insert_geom(struct obj_model* from_obj, struct obj_model* to_obj);

void show(struct obj_model* objmodel);
void show(struct obj_info* obinfo);
void show_obj_geom(struct obj_model* loader);


void get_obj_info(struct obj_model* loader, struct obj_info* obinfo);
void reset_objfile(struct obj_model* loader, struct obj_info* obinfo);
void load_objfile( char *filepath, struct obj_model* loader);
void save_objfile( char *filepath, struct obj_model* loader);

void test_loader_data(struct obj_model* loader);








#endif
