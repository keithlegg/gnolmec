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


//Cautiously moved to the heap  
const int num_vtx   = 120000;
const int num_faces = 120000;


// index/indices for a line object 
typedef struct line{
    int pt1;
    int pt2;
};

// index/indices for a triangle polygon
typedef struct triangle{
    int pt1;
    int pt2;
    int pt3;    
};

// index/indices for a quad polygon 
typedef struct quad{
    int pt1;
    int pt2;
    int pt3;    
    int pt4;
};

/*
    // store info about an object , bbox, etc 
    typedef struct obj_info{
        int num_pts;
        int num_nrmls;
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
*/


/*
    // 3D object in its full glory 
    typedef struct obj_model{
        int num_pts;
        int num_nrmls;    
        int num_uvs;

        int num_lines;
        int num_tris;
        int num_quads;    

        struct vec2 uvs[num_vtx];          // UV coords 
        struct vec3 points[num_vtx];       // 3 floats  
        struct vec3 normals[num_vtx];      // 3 floats 

        //struct vec3 vtx_colors[1000];   

        struct line lines[num_faces];       //2 point lines 
        struct triangle tris[num_faces];    //3 point polygons 
        struct quad quads[num_faces];       //4 point polygons 
    };
*/


class obj_model {
    public:
        obj_model(){
            num_pts    = 0;
            num_vnrmls  = 0;
            num_fnrmls  = 0;
            num_uvs    = 0;
            num_lines  = 0;
            num_tris   = 0;
            num_quads  = 0; 
            num_vtxrgb = 0;            
        };

        ~obj_model(){};

        int num_pts;
        int num_vtxrgb;
        int num_vnrmls;    
        int num_fnrmls;
        int num_uvs;
        int num_lines;
        int num_tris;
        int num_quads;    

        struct vec3 points[num_vtx];        // 3 floats - vertex  
        struct vec3 vtxrgb[num_vtx];        // 3 floats - color per vertex 
        struct vec2 uvs[num_vtx];           // 2 floats - UV coords 
        struct vec3 vnormals[num_vtx];      // 3 floats - face normal 
        struct vec3 fnormals[num_faces];    // 3 floats - face normal 
        struct line lines[num_faces];       // 2 ints   - lines    idx
        struct triangle tris[num_faces];    // 3 ints   - triangle idx 
        struct quad quads[num_faces];       // 4 ints   - quad     idx 

        //vec3 get_obj_centroid( void );

};


class obj_info {
    public:
        obj_info(){

        };

        ~obj_info(){};

    int num_pts;
    int num_nrmls;
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


vec3 get_extents( obj_info* obinfo );


//void gen_normals():

//void insert_geom(obj_model* from_obj, obj_model* to_obj);

void show(obj_model* objmodel);
void show(obj_info* obinfo);
void show_obj_geom(obj_model* loader);


void get_obj_info(obj_model* loader, obj_info* obinfo);
void reset_objfile(obj_model* loader, obj_info* obinfo);

void load_objfile( char *filepath, obj_model* loader);

void save_objfile( char *filepath, obj_model* loader);

void test_loader_data( obj_model* loader);








#endif
