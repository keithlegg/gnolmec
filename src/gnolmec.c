
/*************************************************************/
/*
   gnolmec.c 

       make a 3D environment to view models and play in that is navigatable in the way Maya does it
       Named Olmec - because its not Maya :) 


       -------------------------------------------------------------------------------------
       Maya coordinate system    |  Blender coordinate system  |    Fusion 360 Coordinate System
                                 |                             |  
            Y                    |        Z                    |  
            |                    |        |                    |      
            |                    |        |                    | 
            ______X (look on Z)  |        ____X (look on Y)    |
       -------------------------------------------------------------------------------------
    

    TODO:
        - spaces at end of obj file mess up parsing!
          it prefers NO SPACES currently 

        - textures, line color, lighting 
        - auto generate face/vertex normals
        - UV map loading 
        - MOVE DATA OFF STACK AND ONTO HEAP!  
        - load multiple objects in scene ( layers )
        - test with many models 

        - 3D object save 
        - scenegraph

        - FTDI driver, serial port IO  
        - bezier function 
        - Image compositing ALA Ye olden days of Shake  
        - widgets in GL fixed to screen region 


    IDEAS:

         color :
             3 floats 0-1 after each vertex 
             commented out command in obj 



      Copyright (C) 2014-2020 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/
//#include <stdlib.h>
//#include <stdio.h>

#include <vector>

#include <iostream>
using namespace std;


#include <string.h>
#include <unistd.h>      
#include <cmath>

#include "gl_setup.h"

#include "bitmap_io.h" // includes framebuffer.h, colors, etc 
#include "point_op.h"
#include "obj_model.h"
#include "gnolmec.h"
#include "sceneloader.h"

#include "timer.h"



#define LEN(arr) ( (int) (sizeof (arr) / sizeof (arr)[0]) ) 



/***********/
// window related 
extern int window_id;
extern int scr_size_x;
extern int scr_size_y;
extern bool scr_full_toglr;



extern vector<string>  obj_filepaths;


// view prefs 
bool DRAW_POLYS      = TRUE; // is this needed? 

bool draw_scene_geom = TRUE;

bool draw_points_vbo = TRUE; // test of VBO 
bool draw_points     = TRUE;   
bool draw_lines      = TRUE;
bool draw_normals    = FALSE;



bool draw_quads      = TRUE;
bool draw_triangles  = TRUE;

bool draw_grid       = FALSE;
bool draw_cntrgrid   = TRUE;
bool draw_bbox       = FALSE;

bool render_text     = TRUE;
bool show_textures   = TRUE;


/***************************************/
// object related 


extern char* obj_filepath;
extern int num_loaded_obj;

extern vector<vec3> scene_drawvec3;
extern vector<vec3> scene_drawvecclr;


extern int num_drawvec3;

extern vector<vec3> scene_drawpoints;
extern int num_drawpoints;

char *cam_matrix_filepath = "camera_matrix.olm";
char *proj_matrix_filepath = "projection_matrix.olm";


extern GLuint texture[3];



char active_filepath[300];

// OLD STACK WAY - MOVED TO HEAP 
// struct obj_model model_buffer;
// struct obj_model *pt_model_buffer = &model_buffer;
// struct obj_info obj_info;
// struct obj_info* pt_obinfo = &obj_info;
// struct obj_info loader_info;
// struct obj_info *pt_loadernfo = &loader_info;
// OLD STACK WAY - MOVED TO HEAP 

//my mind is blown... C++ Class with structs in it seems to work!
obj_model * pt_model_buffer = new(obj_model);
obj_model * pt_loader       = new(obj_model);

obj_info* pt_obinfo         = new(obj_info);
obj_info* pt_loadernfo      = new(obj_info);


float xrot, yrot, zrot; 

float obj_len_x, obj_len_y, obj_len_z = 0;

/***************************************/
// data containers 

extern Image* main_bg_bfr      ; 
extern Image* imageloaded_bfr2 ; 
extern Image* imageloaded_bfr  ; 


RGBType line_color;
RGBType *pt_linecolor = &line_color;

RGBType line_color2;
RGBType *pt_linecolor2 = &line_color2;

RGBType grid_color;
RGBType *pt_gridcolor = &grid_color;

RGBType grid_color2;
RGBType *pt_gridcolor2 = &grid_color2;



float ticker = 0;


timer tb;


/***************************************/
// camera properties 


// static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
// static int g_yClick = 0;
// float total_orbitx;

int VIEW_MODE = -1; 


bool toglr_flatshaded = FALSE;
bool view_ismoving = FALSE;
bool mouseLeftDown = FALSE;
bool mouseRightDown = FALSE;
float mouseX, mouseY = 0.0;


float orbit_x;         // 2d click to set 3d view 
float orbit_y;   
float orbit_dist = 5.0; // Z zoom 

// float cam_rotx = 0; // camera rotation
// float cam_roty = 0;
// float cam_rotz = 0;

float cam_posx = 0; // camera location
float cam_posy = 0;
float cam_posz = 0;


int surfce_clr_r; //read from setup.olm 
int surfce_clr_g; 
int surfce_clr_b; 


float moveSpeed    = 2.1f;
/*
float zoomSpeed    = 1.2f;
float rotateSpeed  = 4.0f;
vec3 startpos = newvec3(0.0, 130.0, 60.0);

m33 capsuleObj; //represents a Unity/Maya Transform node 
quaternion orbt_rot_original;
vec3 orbt_xform_original;
*/


/***************************************/

GLfloat emis_full[] = { 1, 1, 1, 0};
GLfloat emis_text[] = { .8, .8, .9, 0};
GLfloat emis_points[] = { 0, .6, .2, 0};
GLfloat emis_off[] = { 0, 0, 0, 0};
GLfloat emis_lines[] = { .5, 0, .5, 0};
GLfloat clr_yellow[] = { 1., 1., 0, 0};


void set_colors(void){
    //peripheral grid color  
    pt_gridcolor->r = 1;
    pt_gridcolor->g = 105;
    pt_gridcolor->b = 5;

    //centergrid gnomon color  
    pt_gridcolor2->r = 200;
    pt_gridcolor2->g = 200;
    pt_gridcolor2->b = 200;

    // geom color  
    pt_linecolor->r = 100;
    pt_linecolor->g = 100;
    pt_linecolor->b = 100;

    // line color 
    // pt_linecolor2->r = 0;
    // pt_linecolor2->g = 100;
    // pt_linecolor2->b = 0;
}



/***************************************/
int num_pts_drw = 0;
GLfloat vertices[100];





/***************************************/
int mouseClickCount = 0;
int rectPlotted;



//WE ARE DOING VERY BAD STUFF HERE - MIXING C++ AND C 
void load_scene(char * scenepath){
    read_scenefile( scenepath );
    char char_array[100];
    
    int x = 0;

    for(x=0;x<num_loaded_obj;x++)
    {

        cout << "paths " << obj_filepaths[x] <<"\n";
        strcpy(char_array, obj_filepaths[x].c_str()); 
        load_objfile(char_array , pt_model_buffer );

        get_obj_info( pt_model_buffer, pt_obinfo);
    }

    calc_normals();
    strcpy(active_filepath, char_array ); 

}

/***************************************/
//DEBUG - GL_MODELVIEW_MATRIX and GL_PROJECTION_MATRIX seem to be the same 
void grab_camera_matrix( m44 *pt_mmm)
{
    GLfloat model[16]; 

    glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();
    glGetFloatv(GL_MODELVIEW_MATRIX, model); 
    //glPopMatrix();

    glutm44_to_m44(pt_mmm, model);

}

//DEBUG - GL_MODELVIEW_MATRIX and GL_PROJECTION_MATRIX seem to be the same 
void grab_projection_matrix(m44 *pt_mpm )
{
    GLfloat model[16]; 

    glMatrixMode(GL_PROJECTION);
    // glPushMatrix();
    //gluOrtho(...);

    glGetFloatv(GL_PROJECTION_MATRIX, model); 
    
    //glPopMatrix();

    glutm44_to_m44(pt_mpm, model);

    // https://www.khronos.org/opengl/wiki/Viewing_and_Transformations
    //manipulate matrix example
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    // glScalef(1., 1., -1.);

    //render_m44(pt_mmm);
    //print_matrix(my_model_matrix);
}


/*
//DEBUG - for testing 
void tweak_matrix( void )
{
 
    GLdouble translate[16] = {1,0,0,0,
                              0,1,0,0,
                              0,0,1,0,
                              1,1,1,1};    


    //glPushMatrix();
    //glMatrixMode(GL_PROJECTION);
    
    //glMatrixMode(GL_MODELVIEW);
                              

    glMultMatrixd(translate);
    //glGetFloatv(GL_PROJECTION_MATRIX, model); 
    
    //glPopMatrix();

    //glutm44_to_m44(pt_mpm, model);

}
*/


void negate_y_axis(m44 *input){
   // this is a hack!  (invert Y is option on renderer! debug )
   // for some reason python 3D renderer wants y inverted  

    float a,b,c;

    a = -input->m1;
    b = -input->m5;
    c = -input->m9;
   
    input->m1 = a;
    input->m5 = b;
    input->m9 = c;
    
}


/***************************************/
/***************************************/



void toggle_polygon_draw(){
    if (DRAW_POLYS == TRUE){
        DRAW_POLYS = FALSE;
        
        draw_quads      = FALSE;
        draw_triangles  = FALSE;

    }else{
        DRAW_POLYS = TRUE;
        
        draw_quads      = TRUE;
        draw_triangles  = TRUE;

    }
}



/***************************************/

void warnings(void)
{
    // let us know if there is a discernable problem 
    printf("\n\n\n\n###########################################\n");

    if(!draw_lines || !pt_model_buffer->num_lines){
        printf("#warn - no lines or disabled.     \n");
    }

    if(!draw_triangles || !pt_model_buffer->num_tris){
        printf("#warn - no triangles or disabled. \n");
    }

    if(!draw_quads || !pt_model_buffer->num_quads){ ; }    

}

/***************************************/

void reset_view(void){
 
    orbit_x    = 0; //.125;  
    orbit_y    = 0; //-.06;   
    orbit_dist = 1; // 5.0;  

    // cam_rotx = 0; //camera rotation
    // cam_roty = 0;
    // cam_rotz = 0;

    cam_posx =  0; //camera location
    cam_posy =  0;
    cam_posz =  0;

}

void calc_normals(void)
{
    int  p_i = 0;
    vec3 tri_cntr;

    //debug - clear any loaded normals 
    pt_model_buffer->num_fnrmls = 0;

    for (p_i=0;p_i<pt_model_buffer->num_tris;p_i++)
    {   
            
        // fetch the pts for a triangle
        vec3 p1 = pt_model_buffer->points[pt_model_buffer->tris[p_i].pt1-1];
        vec3 p2 = pt_model_buffer->points[pt_model_buffer->tris[p_i].pt2-1];
        vec3 p3 = pt_model_buffer->points[pt_model_buffer->tris[p_i].pt3-1];

        // calculate the face normal  
        vec3 a = sub(p1,p2);
        vec3 b = sub(p1,p3);
        vec3 n = normalize(cross(a,b));

        pt_model_buffer->fnormals[pt_model_buffer->num_fnrmls]=n;
        pt_model_buffer->num_fnrmls++;
    }

    // broken experiment to put face normals in vertext normals
    // only do so if nothing was loaded from the model 
    // DEBUG - its wrong !
    if (pt_model_buffer->num_vnrmls==0)
    {
        for (p_i=0;p_i<pt_model_buffer->num_tris;p_i++)
        {    
            // fetch the pts for a triangle
            vec3 p1 = pt_model_buffer->points[pt_model_buffer->tris[p_i].pt1-1];
            vec3 p2 = pt_model_buffer->points[pt_model_buffer->tris[p_i].pt2-1];
            vec3 p3 = pt_model_buffer->points[pt_model_buffer->tris[p_i].pt3-1];

            // calculate the face normal  
            vec3 a = sub(p1,p2);
            vec3 b = sub(p1,p3);
            vec3 n = normalize(cross(a,b));
            pt_model_buffer->vnormals[pt_model_buffer->tris[p_i].pt1-1]= n;
            pt_model_buffer->vnormals[pt_model_buffer->tris[p_i].pt2-1]= n;
            pt_model_buffer->vnormals[pt_model_buffer->tris[p_i].pt3-1]= n;      
            pt_model_buffer->num_vnrmls++;
            pt_model_buffer->num_vnrmls++;
            pt_model_buffer->num_vnrmls++;
    
        }

    }

}
/***************************************/

int q_i, p_i, f_i = 0;


static void render_loop()
{

    // Clear The Screen And The Depth Buffer
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


    if (render_text)
    {
        
        glBindTexture(GL_TEXTURE_2D, texture[0]); 

        glMaterialfv(GL_FRONT, GL_EMISSION, emis_text);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

        char s[100];
        glColor3d(1.0, 1.0, 1.0);
        setOrthographicProjection();
        //glPushMatrix();
        glLoadIdentity();
        void *font = GLUT_BITMAP_8_BY_13;     
        sprintf(s, "    %d polys loaded", pt_model_buffer->num_tris );
        renderBitmapString( ((int)scr_size_x/2)-150 , scr_size_y-20  ,(void *)font, s );
        sprintf(s, "camera position : %f %f %f", cam_posx, cam_posy, cam_posz);
        renderBitmapString( ((int)scr_size_x/2)-150 , scr_size_y-10  ,(void *)font, s );
        //glPopMatrix();
        resetPerspectiveProjection();

        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);
    }

    // --------------------------------------------

    // // glPushMatrix();

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //glTranslatef( sin(tb.getElapsedTime()), 0, 0);
    //glRotatef(45,0,1,0);

    //----------------------------------------------

    // // move the view without using gluLookAt
    // glMatrixMode( GL_MODELVIEW );
    // glLoadIdentity();
    // glTranslatef( 0,0,-10 );
    // glRotatef( 90, 1,0,0 );

        // timer test 
        //tb.stop();
        // print the elapsed time in millisec
 

    //----------------------------------------------
   
    switch (VIEW_MODE) 
    { 
        // orthographic side 
        case 1: 
            //I dont like doing it this way - have the target follow camera 
            gluLookAt( 1.0   , cam_posy , cam_posz,  // look from camera XYZ
                       0.0   , cam_posy , cam_posz,  // look at the origin
                       0.0   , 1.0      , 0.0        // positive Y up vector
            );        
        break; 
    
        // orthographic top   
        case 2:  
            gluLookAt( cam_posx , 1.0   , cam_posz,  // look from camera XYZ
                       cam_posx , 0.0   , cam_posz,  // look at the origin
                       0.0     , 0.0    , -1.0        // positive X (Y up when looking down)
            );   

        break; 
    
        // orthographic front  
        case 3:  

            gluLookAt( cam_posx , cam_posy  , 1.0,   // look from camera XYZ
                       cam_posx , cam_posy  , 0.0,   // look at the origin
                       0.0      , 1.0       , 0.0    // positive Y up vector
            );    
        break; 
    

        default:   
            // cheapo perspective navigation  
            cam_posx = -sin( orbit_x * moveSpeed )     * orbit_dist    ;
            cam_posy =      -orbit_y                   * (orbit_dist/2);
            cam_posz =  cos( orbit_x * moveSpeed )     * orbit_dist    ;

            //for now use gluLookAt for all view modes -  
            gluLookAt( cam_posx, cam_posy , cam_posz,  // look from camera XYZ
                       0.0     , 0.0      , 0.0,       // look at the origin
                       0.0     , 1.0      , 0.0        // positive Y up vector
            ); 

        break;   
    } 
 
    
    /******************************************/
    /******************************************/

    graticulate(&draw_grid, &draw_cntrgrid, pt_gridcolor, pt_gridcolor2);

    show_bbox(&draw_bbox, pt_obinfo, pt_gridcolor);
 
    /******************************************/

    //m33 foobar = m33_from_euler(45,45,45);
    //render_m33(&foobar);
    
    // m44 foobar = m44_from_euler(0,45,90);
    // render_m44(&foobar);

    /******************************************/


    if (draw_points_vbo)
    {
        // persistant point buffer   
        // Not tested well! - I think it needs OpenGL4 amd up

        // This is the first place in Gnolmec that required OpenGL library (not just Glut)

        //http://ogldev.atspace.co.uk/www/tutorial02/tutorial02.html 
        //https://stackoverflow.com/questions/28849321/how-to-draw-polygon-with-3d-points-in-modern-opengl
        //This is the first place in the code that required OpenGL library (not just Glut)
        
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_points);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

        glPointSize(2);
 
        //-------------------------
        // KEEP THIS CODE - 
        // dump to GLFloats 
        num_pts_drw = pt_model_buffer->num_pts;
        GLfloat vertices[num_pts_drw*4];
        GLfloat* pt_vert = vertices;
        dump_points_GLfloat( pt_vert, pt_model_buffer, num_pts_drw );

        //-------------------------


        GLuint VBO;

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        //glEnableVertexAttribArray(0);


        // "Enable a port" to the shader pipeline
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // pass information about how vertex array is composed
        glVertexAttribPointer(0, // same as in glEnableVertexAttribArray(0)
                              4, // # of coordinates that build a vertex
                              GL_FLOAT, // data type
                              GL_FALSE, // normalized?
                              0,        // stride
                              (void*)0);// vbo offset

 
        glDrawArrays(GL_POINTS, 0, num_pts_drw);
        //glDrawArrays(GL_LINES, 0, num_pts_drw);
        glDisableVertexAttribArray(0);
        
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);

    }

    /******************************************/


    /******************************************/

    // draw 3D line geometry 
    if (draw_lines)
    {

        glBindTexture(GL_TEXTURE_2D, texture[0]);

        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
        //glEnable(GL_COLOR_MATERIAL);  
        //glColor3f(.5, 0, .5);
            
        for (p_i=0;p_i<pt_model_buffer->num_lines;p_i++)
        {   
            glBegin(GL_LINES);
                // fetch the line indices from vertex list 
                int lin1 = pt_model_buffer->lines[p_i].pt1;
                int lin2 = pt_model_buffer->lines[p_i].pt2;
                
                vec3 pt1 = pt_model_buffer->points[lin1-1];
                vec3 pt2 = pt_model_buffer->points[lin2-1];

                //use the same vertex indices to lookup RGB 
                vec3 c1 = pt_model_buffer->vtxrgb[lin1-1];
                vec3 c2 = pt_model_buffer->vtxrgb[lin2-1];

                glColor3f(c1.x, c1.y, c1.z);   
                glVertex3f(pt1.x, pt1.y, pt1.z);

                glColor3f(c2.x, c2.y, c2.z);  
                glVertex3f(pt2.x, pt2.y, pt2.z);
            glEnd();
        }

        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);  
    }

    /******************************************/
    // draw the polygon geometry 

    if(draw_triangles)
    {
        //glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 

        //glColor3f( pt_linecolor->r, pt_linecolor->g, pt_linecolor->b); 
        glBindTexture(GL_TEXTURE_2D, texture[2]);   // choose the texture to use.
        
        if (toglr_flatshaded){
            glColor3f(1.,1.,1.);
        }
        
        glBegin(GL_TRIANGLES);  
            for (p_i=0;p_i<pt_model_buffer->num_tris;p_i++)
            { 
                // fetch the triangle indices from vertex list
                int tri1 = pt_model_buffer->tris[p_i].pt1;
                int tri2 = pt_model_buffer->tris[p_i].pt2;
                int tri3 = pt_model_buffer->tris[p_i].pt3;
                
                //use the same vertex indices to lookup RGB 
                vec3 rgb1 = pt_model_buffer->vtxrgb[tri1-1];
                vec3 rgb2 = pt_model_buffer->vtxrgb[tri2-1];
                vec3 rgb3 = pt_model_buffer->vtxrgb[tri3-1];

                /***********************/                
                glColor3f(rgb1.x,rgb1.y,rgb1.z);   

                //vec2 uv = pt_model_buffer->uvs[tri1];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.5, 1.0);                
                vec3 pt1 = pt_model_buffer->points[tri1-1];
                glVertex3f(pt1.x, pt1.y, pt1.z);

              
                vec3 nrm1 = pt_model_buffer->vnormals[tri1-1];
                glNormal3f( nrm1.x, nrm1.y, nrm1.z);
                //printf( " %d %d %d \n",  nrm1.x, nrm1.y, nrm1.z);


                /***********************/ 
                glColor3f(rgb2.x,rgb2.y,rgb2.z); 

                //vec2 uv = pt_model_buffer->uvs[tri2];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.0, 1.0); 

                vec3 pt2 = pt_model_buffer->points[tri2-1];
                glVertex3f(pt2.x, pt2.y, pt2.z);

                // calculated face normals 
                vec3 nrm2 = pt_model_buffer->vnormals[tri2-1];
                glNormal3f( nrm2.x, nrm2.y, nrm2.z);


                /***********************/
                glColor3f(rgb3.x,rgb3.y,rgb3.z); 

                //vec2 uv = pt_model_buffer->uvs[tri3];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(1.0, 0.0);       

                vec3 pt3 = pt_model_buffer->points[tri3-1];
                glVertex3f(pt3.x, pt3.y, pt3.z);

                // calculated face normals
                vec3 nrm3 = pt_model_buffer->vnormals[tri3-1];
                glNormal3f( nrm3.x, nrm3.y, nrm3.z);


            }

        glEnd(); 

        vec3 tri_cntr;

        // display loaded normals as lines 
        if (draw_normals)
        {
            glColor3f(.5,.5,0);

            for (p_i=0;p_i<pt_model_buffer->num_tris;p_i++)
            {             
                // fetch the pts for a triangle
                vec3 p1 = pt_model_buffer->points[pt_model_buffer->tris[p_i].pt1-1];
                vec3 p2 = pt_model_buffer->points[pt_model_buffer->tris[p_i].pt2-1];
                vec3 p3 = pt_model_buffer->points[pt_model_buffer->tris[p_i].pt3-1];

                // calculate the centroid 
                tri_cntr.x = (p1.x + p2.x + p3.x)/3;
                tri_cntr.y = (p1.y + p2.y + p3.y)/3;
                tri_cntr.z = (p1.z + p2.z + p3.z)/3; 
                  
                vec3 mv =  add(tri_cntr, div(pt_model_buffer->fnormals[p_i],5 ));
                //vec3 mv =  add(tri_cntr, pt_model_buffer->fnormals[p_i]);

                //print_vec3( mv );

                glBindTexture(GL_TEXTURE_2D, texture[0]);
                glMaterialfv(GL_FRONT, GL_EMISSION, clr_yellow);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

                    glBegin(GL_LINES);
                        glVertex3f(tri_cntr.x, tri_cntr.y, tri_cntr.z);
                        glVertex3f(mv.x, mv.y, mv.z);
                    glEnd();


                glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);  
            }
        }        

    }

    // ----------------------
    // draw scene defined geom 
    if (draw_scene_geom )
    {

        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_full);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

        for (p_i=0;p_i<num_drawvec3;p_i++)
        {   
            vec3 dv  = scene_drawvec3[p_i];
            vec3 rgb = scene_drawvecclr[p_i];            
            //print_vec3(dv);

            glBegin(GL_LINES);
                glColor3f(rgb.x,rgb.y,rgb.z);
                glVertex3f(0,0,0);

                glColor3f(rgb.x,rgb.y,rgb.z);
                glVertex3f(dv.x, dv.y, dv.z);
        
            glEnd();
        }
        
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 

    }

    //-----------------------

    //DEBUG - IF THERE IS A SPACE AT END OF FIDS, IT LOADS TRI AS QUAD
    if (draw_quads )
    {
        
        if (toglr_flatshaded){
            glColor3f(1.,1.,1.);
        }

        glBindTexture(GL_TEXTURE_2D, texture[3]);

        glBegin(GL_QUADS);                      

            for (q_i=0;q_i<pt_model_buffer->num_quads;q_i++)
            { 

                int qu1 = pt_model_buffer->quads[q_i].pt1;
                int qu2 = pt_model_buffer->quads[q_i].pt2;
                int qu3 = pt_model_buffer->quads[q_i].pt3;
                int qu4 = pt_model_buffer->quads[q_i].pt4;

                //DEBUG VTX COLORS ARE BROKEN - INDEXING ISSUES 
                vec3 rgb1 = pt_model_buffer->vtxrgb[qu1-1];
                vec3 rgb2 = pt_model_buffer->vtxrgb[qu2-1];
                vec3 rgb3 = pt_model_buffer->vtxrgb[qu3-1];
                vec3 rgb4 = pt_model_buffer->vtxrgb[qu4-1];

                /***********************/
                glColor3f(rgb1.x,rgb1.y,rgb1.z);                 
                glTexCoord2f(0.5, 1.0);                
                vec3 pt1 = pt_model_buffer->points[qu1-1];
                glVertex3f(pt1.x, pt1.y, pt1.z);

                /***********************/
                glColor3f(rgb2.x,rgb2.y,rgb2.z); 
                glTexCoord2f(0.0, 1.0); 
                vec3 pt2 = pt_model_buffer->points[qu2-1];
                glVertex3f(pt2.x, pt2.y, pt2.z);

                /***********************/
                glColor3f(rgb3.x,rgb3.y,rgb3.z);                 
                glTexCoord2f(1.0, 0.0);                
                vec3 pt3 = pt_model_buffer->points[qu3-1];
                glVertex3f(pt3.x, pt3.y, pt3.z);

                /***********************/
                glColor3f(rgb4.x,rgb4.y,rgb4.z);                 
                glTexCoord2f(1.0, 0.0);                
                vec3 pt4 = pt_model_buffer->points[qu4-1];
                glVertex3f(pt4.x, pt4.y, pt4.z);


            }
        

        glEnd(); 




    }


    // glPopMatrix();

    // swap the other (double) buffer to display what just got drawn.
    glutSwapBuffers();
  
    view_ismoving = FALSE;
}





/***************************************/
void animate(){
    //ticker += .01;
    render_loop();
}
/***************************************/

static void reshape_window(int width, int height)
{
    
    if ( height == 0 )                // Prevent A Divide By Zero If The Window Is Too Small
        height=1;
    glViewport(0, 0, width, height);  // Reset The Current Viewport And Perspective Transformation

    scr_size_x = width;
    scr_size_y = height;    


    
    // // https://www.khronos.org/opengl/wiki/Viewing_and_Transformations
    // int fov = 1;
    // int zNear = .1;
    // int zFar = 10;
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // float aspectYScale = 1.0;
    // if ( aspect < conditionalaspect ) {
    //     aspectYScale *= (float)scr_size_x / (float)scr_size_y / conditionalaspect;
    // }
    // gluPerspective(atan(tan(fov * 3.14159 / 360.0) / aspectYScale) * 360.0 / 3.14159, (float)scr_size_x/(float)scr_size_y, zNear, zFar);

}//end resize callback

/***************************************/

void set_view_ortho(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                   
    
    //gluOrtho2D(scr_size_x, scr_size_y, -1, 1 ); //(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
    float orthzoom = 200; // orbit_dist*200;
    glOrtho( -scr_size_x/orthzoom, scr_size_x/orthzoom, -scr_size_y/orthzoom, scr_size_y/orthzoom, -1, 1 );

    glMatrixMode(GL_MODELVIEW);


    // // set viewport to be the entire window
    // glViewport(0, 0, (GLsizei)scr_size_x, (GLsizei)scr_size_y);
    // // set orthographic viewing frustum
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();
    // glOrtho(0, scr_size_x, 0, scr_size_y, -1, 1);
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();

}

//text demo 
void setOrthographicProjection() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
       gluOrtho2D(0, scr_size_x, 0, scr_size_y);
    glScalef(1, -1, 1);
    glTranslatef(0, -scr_size_y, 0);
    glMatrixMode(GL_MODELVIEW);
} 

//text demo 
void resetPerspectiveProjection() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
} 

//text demo 
void renderBitmapString(float x, float y, void *font,const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
} 




/***************************************/
void set_view_persp(void)
{
    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)(scr_size_x)/scr_size_y, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    

}




/**************************************************/
/**************************************************/
/**************************************************/

void olmec(int *argc, char** argv){
 
    glutInit(argc, argv);  

    //shader_test();
    set_colors();

    load_scene(obj_filepath);

    warnings();

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(scr_size_x, scr_size_y);  //window size
    glutInitWindowPosition(0, 0);  
    
    window_id = glutCreateWindow("Olmec v.b0002.80"); //create an opengl window 

    /***********/
    reset_view();

    //register GL callbacks       

    glutDisplayFunc(&render_loop); 
    glutIdleFunc(&animate);

    glutReshapeFunc(&reshape_window);  // register window resize callback 

    glutKeyboardFunc(&keyPressed);     // register key pressed callback 

    InitGL(scr_size_x, scr_size_y); // Initialize window. 
  

    // experimental draw polygon 
    //glutMouseFunc (draw_poly_mousevent);

    glutMouseFunc (olmec_mouse_button);
    glutMotionFunc (olmec_mouse_motion);




    loadImage("textures/generated2.bmp" , imageloaded_bfr);
    loadImage("textures/generated2.bmp" , imageloaded_bfr2);


    //copyBuffer24( imageloaded_bfr, loaded_texture ); //convert "RGBType" to "Image"

    // create and apply 2D texture   
    glGenTextures(4, &texture[0]);            //create 3 textures

    //grid color
    // glBindTexture(GL_TEXTURE_2D, texture[0]);  
    // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // scale linearly when image bigger than texture
    // glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // scale linearly when image smalled than texture
    
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, imageloaded_bfr->sizeX, imageloaded_bfr->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, imageloaded_bfr2->data);


    //general line color 
    glBindTexture(GL_TEXTURE_2D, texture[1]);  
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // scale linearly when image bigger than texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // scale linearly when image smalled than texture
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, imageloaded_bfr2->sizeX, imageloaded_bfr2->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, imageloaded_bfr2->data);


    //polygon color 
    glBindTexture(GL_TEXTURE_2D, texture[2]);  
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // scale linearly when image bigger than texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imageloaded_bfr->sizeX, imageloaded_bfr->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, imageloaded_bfr2->data);


    //polygon color 2
    glBindTexture(GL_TEXTURE_2D, texture[3]);  
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // scale linearly when image bigger than texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, imageloaded_bfr->sizeX, imageloaded_bfr->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, imageloaded_bfr->data);

    ///////////
    
    //tb.start(); //test of timer 

    glutMainLoop();// Start Event Processing Engine   


}



/**************************************************/
/**************************************************/


    //var wheelie = Input.GetAxis("Mouse ScrollWheel");
        
    // if (wheelie < 0) // back
    // {
    //     var currentZoomSpeed = 100f;
    //     transform.Translate(Vector3.forward * (wheelie * currentZoomSpeed));
    // }
    // if (wheelie > 0) // back
    // {
    //      var currentZoomSpeed = 100f;
    //      transform.Translate(Vector3.forward * (wheelie * currentZoomSpeed));
    // }

    /*
    //Input.GetAxis("Mouse ScrollWheel") < 0) // back
    if( Input.GetKey(KeyCode.RightAlt) || Input.GetKey(KeyCode.LeftAlt) ){

      // Distance between camera and orbitVector. We'll need this in a few places
      var distanceToOrbit = Vector3.Distance(transform.position, orbitVector.transform.position);
    
        //RMB - ZOOM
        if (Input.GetMouseButton(1)) {
            
            // Refine the rotateSpeed based on distance to orbitVector
            var currentZoomSpeed = Mathf.Clamp(zoomSpeed * (distanceToOrbit / 50), 0.1f, 2.0f);
            
            // Move the camera in/out
            transform.Translate(Vector3.forward * (x * currentZoomSpeed));
            
            // If about to collide with the orbitVector, repulse the orbitVector slightly to keep it in front of us
            if (Vector3.Distance(transform.position, orbitVector.transform.position) < 3) {
                orbitVector.transform.Translate(Vector3.forward, transform);
            }

        
        //LMB - PIVOT
        } else if (Input.GetMouseButton(0)) {
            
            // Refine the rotateSpeed based on distance to orbitVector
            var currentRotateSpeed = Mathf.Clamp(rotateSpeed * (distanceToOrbit / 50), 1.0f, rotateSpeed);
            
            
            // Temporarily parent the camera to orbitVector and rotate orbitVector as desired
            transform.parent = orbitVector.transform;
            orbitVector.transform.Rotate(Vector3.right * (y * currentRotateSpeed));
            orbitVector.transform.Rotate(Vector3.up * (x * currentRotateSpeed), Space.World);
            transform.parent = null;
                    
        //MMB - PAN
        else if (Input.GetMouseButton(2)) {
            
            // Calculate move speed
            var translateX = Vector3.right * (x * moveSpeed) * -1;
            var translateY = Vector3.up * (y * moveSpeed) * -1;
            
            // Move the camera
            transform.Translate(translateX);
            transform.Translate(translateY);
            
            // Move the orbitVector with the same values, along the camera's axes. In effect causing it to behave as if temporarily parented.
            orbitVector.transform.Translate(translateX, transform);
            orbitVector.transform.Translate(translateY, transform);
        }
    */        

// Use this for initialization
void olmecnav_start (void ) {
    // Create a transform (which will be the lookAt target and global orbit vector)
    
    //     capsuleObj = GameObject.CreatePrimitive(PrimitiveType.Capsule);
    //     capsuleObj.transform.position = Vector3.zero;

    // Snap the camera to align with the grid in set starting position (otherwise everything gets a bit wonky)
   
    // transform.position = startpos;  
    // transform.LookAt(capsuleObj.transform.position, Vector3.up);
    // capsuleObj.renderer.enabled = false; //hide the capsule object     

    //     ///
    //     orbt_xform_original = capsuleObj.transform.position;
    //     orbt_rot_original   = capsuleObj.transform.rotation;
}


/**************************************************/
/**************************************************/

/*
void add_draw_pt(float xx, float yy){
    vertices[num_pts_drw] = GLfloat(xx);
    num_pts_drw++;
    vertices[num_pts_drw] = GLfloat(yy);
    num_pts_drw++;
    vertices[num_pts_drw] = GLfloat(0);
    num_pts_drw++;
    vertices[num_pts_drw] = GLfloat(1.0);
    num_pts_drw++;
}

  void on_vertex_selected(int x, int y){

     if(mouseClickCount == 0){
       
        printf("## X %d Y %d \n", x, y );

        // glVertex2i(dr_x1, dr_y1);
        // glVertex3f (v1.x, v1.y, v1.z);
       

       add_draw_pt(2.0, 1.7);

     }
     else{
        vertices[num_pts_drw] = GLfloat(x);
        num_pts_drw++;
        vertices[num_pts_drw] = GLfloat(y);
        num_pts_drw++;
        vertices[num_pts_drw] = GLfloat(1.0);
        num_pts_drw++;
        vertices[num_pts_drw] = GLfloat(1.0);
        num_pts_drw++;

        // glVertex2i(dr_x1, dr_y1);
        // glVertex2i(dr_x1, dr_y2);
        // glVertex2i(dr_x2, dr_y2);
        // glVertex2i(dr_x2, dr_y1);

     }
}


void draw_poly_mousevent(int button, int state, int x, int y){
   if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN && mouseClickCount == 0){
      //y = y+20; //adjusts for VM mouse tracking error

      on_vertex_selected(x, scr_size_y - y);
      rectPlotted = 0;
   }

   if(button==GLUT_LEFT_BUTTON && state ==GLUT_UP && mouseClickCount == 0){
      if(rectPlotted == 1){
       return;
      }
      else{
       mouseClickCount++;
      }
   }

   if(button==GLUT_LEFT_BUTTON && state ==GLUT_DOWN && mouseClickCount == 1){
       //y = y+20; //adjusts for VM mouse tracking error
       on_vertex_selected(x, scr_size_y - y);
       mouseClickCount = 0;
       rectPlotted = 1;
   }
}
*/

void olmec_mouse_button(int button, int state, int x, int y)
{

    mouseX = x;
    mouseY = y;

    //left click 
    if (button == GLUT_LEFT_BUTTON)
      {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;

      }

      // middle click
      if ((button == 3) || (button == 4)) // It's a wheel event
      {
           // Disregard redundant GLUT_UP events
           if (state == GLUT_UP) return; 

           if (button == 3){
               //if (orbit_dist < -1.5){
               orbit_dist+=.1;  
               //printf("# orbit dist %f \n", orbit_dist );                                 
               //}
  
           }
           if (button == 4){
               //if (orbit_dist>0){ 
                   orbit_dist-=.1; 
               //}
           }
      }else{  // normal button event
           if (state == GLUT_DOWN){
               // printf("olmec middle click\n");  
           }
      }


    //Right click
    if (button == GLUT_RIGHT_BUTTON)
      {
        
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;

      }

}



/********************************************/

float coefficient = 0.005f;     

void olmec_mouse_motion(int x, int y)
{
    // take offset from center of screen to get "X,Y delta"
    float center_y = (float)scr_size_y/2;
    float center_x = (float)scr_size_x/2;


    if(mouseLeftDown)
    { 

        switch (VIEW_MODE) 
        { 

            // orthographic side  (key 2)
            case 1: 
                view_ismoving = TRUE;
                cam_posz = -(center_x-x) * coefficient; 
                cam_posy = -(center_y-y) * coefficient; 
            break; 
        
            // orthographic top   (key shift 2)
            case 2:  
                view_ismoving = TRUE;
                cam_posx = (center_x-x) * coefficient; 
                cam_posz = (center_y-y) * coefficient;  
            break; 
        
            // orthographic front  (key 3)
            case 3:  
                view_ismoving = TRUE;
                cam_posx = (center_x-x)  * coefficient; 
                cam_posy = -(center_y-y) * coefficient; 
            break; 
        

            default:  
                    view_ismoving = TRUE;
                    
                    orbit_x += (x-mouseX) * coefficient; 
                    orbit_y += (y-mouseY) * coefficient; 
                    mouseX = x;
                    mouseY = y;
                    //printf("# mouse motion %d %d %f %f \n", x,y, orbit_x, orbit_y );
            break;
        }
    }

    if(mouseRightDown)
    {
        orbit_dist -= (y - mouseY) * 0.02f;
        mouseY = y;
    }

    /**************/
   

     

}



/*
    // a - frame all objects 
    // q - select tool         
    // w - move 
    // e - rotate 
    // r - scale 
    // f - frame selected 
    // t - show manipulator 
    // p - parent 
    // shft p - unparent 
*/

void setlight0(void){
    // FROM - https://www.khronos.org/opengl/wiki/How_lighting_works#glMaterial_and_glLight 
    // FROM - https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glLight.xml
    // FROM - https://www.cse.msu.edu/~cse872/tutorial3.html


    /* 
        GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_POSITION, GL_SPOT_CUTOFF, GL_SPOT_DIRECTION, 
        GL_SPOT_EXPONENT, GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION,  GL_QUADRATIC_ATTENUATION 
    */

    GLfloat lightpos[] = {1., 1., 1., 0.};
    glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

    // Set GL_LIGHT_0's Ambient color to 0,0,0,1
    GLfloat lightamb[] = {0., 0., 0., 1. };
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightamb);

    // Set GL_LIGHT_0's Diffuse color to 1,1,1,1
    GLfloat lightdif[] = {1., 1., 1., 1. };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightdif);

    // Set GL_LIGHT_0's Specular color to 1,1,1,1
    GLfloat lightspec[] = {1., 1., 1., 1. };
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightspec);


    // Set the glLightModel global ambient to 0.2,0.2,0.2,1 (this is the default).
    // Don't set any other glLight or glLightModel options - just let them default.

    // Enable GL_LIGHTING and GL_LIGHT_0.
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Enable GL_COLOR_MATERIAL and set glColorMaterial to GL_AMBIENT_AND_DIFFUSE. 
    // This means that glMaterial will control the polygon's specular and emission colours 
    // and the ambient and diffuse will both be set using glColor.
    
    // glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    // glEnable(GL_COLOR_MATERIAL);  

    // Set the glMaterial Specular colour to 1,1,1,1
    // GLfloat matspec[] = {1.f, 1.f, 1.f, 1.f};
    // glMaterialfv(GL_FRONT, GL_SPECULAR, matspec);

    // GLfloat matemis[] = {1.f, 0.f, 0.f, 0.f};
    // glMaterialfv(GL_FRONT, GL_EMISSION, matemis);

    // Set the glMaterial Emission colour to 0,0,0,1
    // Set the glColor to whatever colour you want each polygon to basically appear to be. 
    // That sets the Ambient and Diffuse to the same value - which is what you generally want.


    //  // GLfloat matdiff[] = {1.f, 1.f, 1.f, 1.f};
    //  // glMaterialfv(GL_FRONT, GL_DIFFUSE, matdiff);


}



//define keyboard events 
static void keyPressed(unsigned char key, int x, int y) 
{

    //printf("scancode key %u \n", key );

    usleep(100);




    //ESCAPE KEY
    if (key == 27) 
    { 
        glutDestroyWindow(window_id); 
        exit(0);                   
    }

    //d key 
    if (key == 100) 
    { 
        toggle_polygon_draw();
    }

    if (key == 49) //1 - perspective mode
    { 
      
        VIEW_MODE = -1; 

        // // set viewport to be the entire window
        // glViewport(0, 0, (GLsizei)scr_size_x, (GLsizei)scr_size_y);
        // // set perspective viewing frustum
        // glMatrixMode(GL_PROJECTION);
        // glLoadIdentity();
        // gluPerspective(45.0f, (float)(scr_size_x)/scr_size_y, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip
        // // switch to modelview matrix in order to set scene
        // glMatrixMode(GL_MODELVIEW);
        // glLoadIdentity();
        
        set_view_persp();


    }

    if (key == 50) //2 - orthographic side 
    { 
        reset_view();
        VIEW_MODE = 1; 
        set_view_ortho();
    }

    if (key == 64) //shift 2 - orthographic top  
    {
        reset_view();
        VIEW_MODE = 2; 
        set_view_ortho();    
    }


    if (key == 51) //3 - orthographic front 
    { 
        reset_view();
        VIEW_MODE = 3; 
        set_view_ortho();
    }

    if (key == 35) //shift 3 - ???
    {
        reset_view();        
        VIEW_MODE = 4; 
        set_view_ortho();        
    }


    if (key == 36) //Shift 4 - display as points 
    { 
        // to draw points  
        //glPolygonMode (GL_FRONT_AND_BACK, GL_POINT);
        //glPointSize(4);
    
        if (draw_points == TRUE){
            glPolygonMode (GL_FRONT_AND_BACK,  GL_POINT);
            draw_points = FALSE;
        }else{
            glPolygonMode (GL_FRONT_AND_BACK,  GL_LINE);
            draw_points = TRUE;
        }


    }

    if (key == 52) //4 - display as wire 
    { 
        glDisable(GL_TEXTURE_2D);        
        glDisable(GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK,  GL_LINE);

    }

    
    if (key == 37) //shift 5 , ignore lights  
    { 
        //trying to get the flat, no shaded ambient look 
        if (toglr_flatshaded == TRUE){
            glDisable(GL_TEXTURE_2D);        
            glDisable(GL_LIGHTING);
            toglr_flatshaded = FALSE;
        }else{
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_LIGHTING);
            toglr_flatshaded = TRUE;
        }



    } 


    if (key == 53) //5 - display as solid, no texture  
    { 

        setlight0();

        // glShadeModel(GL_SMOOTH);              
        glDisable(GL_TEXTURE_2D);

        //------------------------

        //glDisable(GL_COLOR_MATERIAL);

        // glDisable(GL_LIGHT0);
        // glDisable(GL_LIGHT1);
        // glDisable(GL_LIGHT2);
        // glDisable(GL_LIGHT3);
        // glDisable(GL_LIGHT4);
           
        //------------------------
                        
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }
    
    if (key == 54) //6 - display as solid, with texture 
    { 
        glEnable(GL_TEXTURE_2D);

        setlight0();

        //glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }

    if (key == 94) //shift 6 - toggle texture 
    { 
        if (draw_points_vbo == TRUE)
        {
            draw_points_vbo = FALSE;
        }else{
            draw_points_vbo = TRUE;
        }
    }

    if (key == 55) // 7
    { 

        glEnable(GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }

    //------

    if (key == 45) // minus
    { 
        orbit_dist-=.1;  
    }
    
    if (key == 61) //plus
    { 
        orbit_dist+=.1;  
    }

     //------

    if (key == 32) // space
    { 

        if (scr_full_toglr == TRUE){
            glutFullScreen();
            scr_full_toglr = FALSE;
        }else{
            glutReshapeWindow(800, 800);
            glutPositionWindow(0,0);  
            scr_full_toglr = TRUE;
        }

    }
    

    //------
    if (key == 73) //shift i - show obj info 
    { 

        get_obj_info( pt_model_buffer, pt_obinfo);
        show_obj_geom(pt_model_buffer);
    }


    if (key == 105) //i - draw bbox
    { 
        get_obj_info( pt_model_buffer, pt_obinfo);        
        if (draw_bbox == TRUE){
            draw_bbox = FALSE;
        }else{
            draw_bbox = TRUE;
            show(pt_model_buffer);            
        }
    }


    // if (key == 110) // shift n
    // { 
    //    // TODO generate normals here 
    // }

    if (key == 110) // n
    { 

        if (draw_normals == TRUE){
            draw_normals = FALSE;
        }else{
            draw_normals = TRUE;
              
        }
    }

    if (key == 111) // o
    { 

        //char* file2 = "3d_obj/PYCORE.obj";

        strcpy(active_filepath, "3d_obj/PYCORE.obj");

        //reset_objfile(pt_model_buffer, pt_obinfo);

        load_objfile(active_filepath, pt_model_buffer );
        get_obj_info( pt_model_buffer, pt_obinfo);


    }


    if (key == 84) // shift t key 
    { 
       
        if (show_textures == TRUE)
        {
        
            show_textures = FALSE;
        }else{

            show_textures = TRUE;
        }       
 
    }


    if (key == 116) // t key 
    { 
        m44 cam_matrix = identity44();
        grab_camera_matrix(&cam_matrix);
        //transpose(&foo);
        //negate_y_axis(&foo); //PIL want this (origin is TL, Framebuffer and CPP origin is BL)

        // // probably a terrible idea 
        // cam_matrix.m12=cam_posx;
        // cam_matrix.m13=cam_posy;
        // cam_matrix.m14=cam_posz;
        
        print_matrix(cam_matrix);

        save_matrix44( cam_matrix_filepath, &cam_matrix );


        //hmm, this returns the same matrix .. investigate 
        m44 proj_matrix = identity44();
        grab_projection_matrix(&proj_matrix );
        save_matrix44( proj_matrix_filepath, &cam_matrix );

    }
  

    if (key == 80) //shift p
    { 

        //go ahead and dump camera matrix automatically 
        m44 foo = identity44();
        grab_camera_matrix(&foo);
        negate_y_axis(&foo);
        save_matrix44(cam_matrix_filepath, &foo );

        python_render();
    }

    if (key == 112) //p
    { 

        //go ahead and dump camera matrix automatically 
        m44 foo = identity44();
        grab_camera_matrix(&foo);
        negate_y_axis(&foo);
        save_matrix44(cam_matrix_filepath, &foo );

        //launch python3 
        init_pycore(); 
    }


    if (key == 8 || key == 127) //backspace /delete on OSX )
    { 
        m44 cam_matrix = identity44();
        grab_camera_matrix(&cam_matrix);
        //negate_y_axis(&foo); - PIL, CPP and framebuffer use different origins!

        //// probably a terrible idea 
        // cam_matrix.m12=cam_posx;
        // cam_matrix.m13=cam_posy;
        // cam_matrix.m14=cam_posz;

        save_matrix44(cam_matrix_filepath, &cam_matrix );

        //hmm, this returns the same matrix .. investigate 
        //grab_projection_matrix(&foo);
        //save_matrix44("projection_matrix.olm", &foo );

        software_render();
    }
    
    //------
    if (key == 70) //shift f
    { 
        if (draw_scene_geom == TRUE){
            draw_scene_geom = FALSE;

        }else{
            draw_scene_geom = TRUE;
        }
    }

    //------
    if (key == 82) //shift r
    { 
        //cout << "PATHS " << obj_filepaths.clear() << "\n";
        num_loaded_obj = 0;
        clear_scenegeom();
        reset_objfile(pt_loader      , pt_obinfo);// DEBUG why is the same info object here?
        reset_objfile(pt_model_buffer, pt_obinfo);// DEBUG why is the same info object here?

    }

    if (key == 114) //r
    { 
        glColor3f( 1., 1., 1.); 
        load_scene(obj_filepath);
    }


    if (key == 102) //f
    { 
        //TODO - set orbit_dist to X2 model size  
        reset_view();
    }

    if (key == 71) //shift g
    { 
        if (draw_cntrgrid == TRUE){
            draw_cntrgrid = FALSE;
        }else{
            draw_cntrgrid = TRUE;
        }

    }

    if (key == 103) //g
    { 
        if (draw_grid == TRUE){
            draw_grid = FALSE;
        }else{
            draw_grid = TRUE;
        }
    }

}




/**************************************************/
/**************************************************/
/**************************************************/
/**************************************************/




void software_render(void){

    // RUN THE CPP RENDERER    
    set_screen_square(&scr_size_x, &scr_size_y);
    char buffer[512];

    // printf("./renderthing %d %d %s %s %d %d %d %s\n", scr_size_x, scr_size_y, active_filepath, cam_matrix_filepath, 0, 0, 0, "cpp_render.bmp");
    // snprintf(buffer, sizeof(buffer), "./renderthing %d %d %s %s %d %d %d %s", scr_size_x, scr_size_y, active_filepath, cam_matrix_filepath, 0, 0, 0, "cpp_render.bmp"); 
    
    // we need to export a render.olm file here 
    char* scenefilepath = "render.olm";
    write_scenefile(active_filepath, cam_matrix_filepath, scenefilepath );

    printf("./renderthing %d %d %s %s\n", scr_size_x, scr_size_y, scenefilepath, "cpp_render.bmp");
    snprintf(buffer, sizeof(buffer), "./renderthing %d %d %s %s", scr_size_x, scr_size_y, scenefilepath, "cpp_render.bmp");

    int ret = system(buffer);

}


void python_render(void){

    char* pycore_cmd = "scanline";    
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "python3 pycore.py %s %s", active_filepath, pycore_cmd);
    int ret = system(buffer);

}



void init_pycore(void){
    //call python from here!!

    char* pycore_cmd = "runcommand";    
    char buffer[256];

    snprintf(buffer, sizeof(buffer), "python3 pycore.py %s %s", active_filepath, pycore_cmd);
    int ret = system(buffer);

    //load the result in !!
    //char* newfilepath = "3d_obj/normals.obj";
    //load_objfile( newfilepath, pt_model_buffer ); 
}


/**************************************************/

/*
       working - Draw example 

        glPointSize(4);
        
        GLfloat vertices[] =
        {
            1.0f , 3.0f, 1.0f, 1.0f,
            2.0f , 2.0f, 1.0f, 1.0f,
            3.0f , 1.0f, 2.0f, 1.0f
        };

        GLuint VBO;

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        //glEnableVertexAttribArray(0);


        // "Enable a port" to the shader pipeline
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // pass information about how vertex array is composed
        glVertexAttribPointer(0, // same as in glEnableVertexAttribArray(0)
                              4, // # of coordinates that build a vertex
                              GL_FLOAT, // data type
                              GL_FALSE, // normalized?
                              0,        // stride
                              (void*)0);// vbo offset

 
        glDrawArrays(GL_POINTS, 0, 3);
        //glDrawArrays(GL_LINES, 0, 2);
        glDisableVertexAttribArray(0);
*/

