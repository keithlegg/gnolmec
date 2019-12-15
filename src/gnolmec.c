
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
        - textures, line color, lighting 
        - obj model_buffer - get object info 
        - obj model_buffer - calc bbox /bsphere 
        - load multiple objects in scene ( layers )  
        - object save 
        - think about a scenegraph   

        - WIDGETS, BUTTONS
        - ON SCREEN TEXT 
        - HANDLE PYCORE ERRORS 
        - PREFS FORMAT 
        - SCENE FORMAT ? (load/save?)
        - TCP socket ? 
        - (its a cheap, gay-menjun)

      Copyright (C) 2019 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>      
#include <cmath>

#include "gl_setup.h"

#include "bitmap_io.h" // includes framebuffer.h, colors, etc 
#include "point_op.h"
#include "obj_model.h"
#include "gnolmec.h"




/***********/
// window related 
extern int window_id;
extern int scr_size_x;
extern int scr_size_y;
extern bool scr_full_toglr;

// view prefs 
bool DRAW_POLYS      = TRUE;

bool draw_points     = TRUE; //not working, test of VBO  
bool draw_lines      = TRUE;

bool draw_quads      = TRUE;
bool draw_triangles  = TRUE;

bool draw_grid       = FALSE;
bool draw_cntrgrid   = TRUE;
bool draw_bbox       = FALSE;

bool render_text     = TRUE;


/***************************************/
// object related 

extern char* obj_filepath;

extern GLuint texture[3];


//struct obj_model draw_points_buffer;
//struct obj_model *pt_draw_points = &draw_points_buffer;

struct obj_model model_buffer;
struct obj_model *pt_model_buffer = &model_buffer;
struct obj_info obj_info;
struct obj_info* pt_obinfo = &obj_info;


struct obj_model loader;
struct obj_model *pt_loader = &loader;
struct obj_info loader_info;
struct obj_info *pt_loadernfo = &loader_info;





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

/***************************************/
// camera properties 


// static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
// static int g_yClick = 0;
// float total_orbitx;

int VIEW_MODE = -1; 


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
/*
   TODO - attempt to pass this matrix to renderthing to use for view 
*/
void grab_camera_matrix(void)
{
    GLfloat model[16]; 
    m44 my_model_matrix;
    m44 *pt_mmm = &my_model_matrix;

    // Test to attach geometry to camera (crappy widgets) 
    //glGetFloatv(GL_MODELVIEW_MATRIX, model); 
    //glutm44_to_m44(pt_mmm, model);

    // view or print it 
    //render_m44(pt_mmm);
    //print_matrix(my_model_matrix);
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

void set_colors(void){
    //make a color for some lines 
    pt_gridcolor->r = 30;
    pt_gridcolor->g = 30;
    pt_gridcolor->b = 30;

    //make a color for some lines 
    pt_gridcolor2->r = 0;
    pt_gridcolor2->g = 20;
    pt_gridcolor2->b = 20;

    //make a color for some lines 
    pt_linecolor->r = 0;
    pt_linecolor->g = 255;
    pt_linecolor->b = 0;

    //make a color for some lines 
    pt_linecolor2->r = 155;
    pt_linecolor2->g = 160;
    pt_linecolor2->b = 50;

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




/***************************************/

int q_i, p_i, f_i = 0;

static void render_loop()
{


    // Clear The Screen And The Depth Buffer
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);   
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


    if (render_text)
    {
        char s[30];
        glColor3d(1.0, 1.0, 1.0);
        setOrthographicProjection();
        glPushMatrix();
        glLoadIdentity();
        void *font = GLUT_BITMAP_8_BY_13;     
        
        //renderBitmapString( ((int)scr_size_x/2)-150 , scr_size_y-10  ,(void *)font,"Gnolmec -Keith Legg 2019");

        sprintf(s, "%f %f %f", cam_posx, cam_posy, cam_posz);
        renderBitmapString( ((int)scr_size_x/2)-150 , scr_size_y-10  ,(void *)font, s );

        

        //renderBitmapString(110, scr_size_y-20  , (void*)font, s);
        //renderBitmapString(210, scr_size_y-10  ,(void *)font,"Esc - Quit");
        glPopMatrix();
        resetPerspectiveProjection();
    }


    //glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();



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

    if (draw_points)
    {
        // persistant point buffer   
        // Not tested well! - I think it needs OpenGL4 amd up

        // This is the first place in Gnolmec that required OpenGL library (not just Glut)

        //http://ogldev.atspace.co.uk/www/tutorial02/tutorial02.html 
        //https://stackoverflow.com/questions/28849321/how-to-draw-polygon-with-3d-points-in-modern-opengl
        //This is the first place in the code that required OpenGL library (not just Glut)
       
       
        glPointSize(4);
 
        //dump to GLFloats 
        int numdr = pt_model_buffer->num_pts;

        GLfloat vertices[numdr*4];
        GLfloat* pt_vert = vertices;

        dump_points_GLfloat( pt_vert, pt_model_buffer, numdr );


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

 
        glDrawArrays(GL_POINTS, 0, numdr);
        //glDrawArrays(GL_LINES, 0, numdr);
        glDisableVertexAttribArray(0);

    }

    /******************************************/
    // draw 3D line geometry 
    if (draw_lines)
    {
        glBindTexture(GL_TEXTURE_2D, texture[1]);   // choose the texture to use.
            
        for (p_i=0;p_i<pt_model_buffer->num_lines;p_i++)
        {   
            glBegin(GL_LINES);
                // fetch the line indices from vertex list 
                int lin1 = pt_model_buffer->lines[p_i].pt1;
                int lin2 = pt_model_buffer->lines[p_i].pt2;
                
                vec3 pt1 = pt_model_buffer->points[lin1-1];
                vec3 pt2 = pt_model_buffer->points[lin2-1];

                glColor3f(pt_linecolor2->r, pt_linecolor2->g, pt_linecolor2->b);   
                glVertex3f(pt1.x, pt1.y, pt1.z);

                glColor3f(pt_linecolor2->r, pt_linecolor2->g, pt_linecolor2->b);   
                glVertex3f(pt2.x, pt2.y, pt2.z);
            glEnd();
        }


    }

    /******************************************/
    // draw the polygon geometry 

    if(draw_triangles)
    {
        glBindTexture(GL_TEXTURE_2D, texture[2]);   // choose the texture to use.

        glBegin(GL_TRIANGLES);  
            for (p_i=0;p_i<pt_model_buffer->num_tris;p_i++)
            { 
                // fetch the triangle indices from vertex list
                int tri1 = pt_model_buffer->tris[p_i].pt1;
                int tri2 = pt_model_buffer->tris[p_i].pt2;
                int tri3 = pt_model_buffer->tris[p_i].pt3;

                
                //vec2 uv = pt_model_buffer->uvs[tri1];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.5, 1.0);                
                vec3 pt1 = pt_model_buffer->points[tri1-1];
                glVertex3f(pt1.x, pt1.y, pt1.z);

                //vec2 uv = pt_model_buffer->uvs[tri2];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.0, 1.0); 
                vec3 pt2 = pt_model_buffer->points[tri2-1];
                glVertex3f(pt2.x, pt2.y, pt2.z);

                //vec2 uv = pt_model_buffer->uvs[tri3];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(1.0, 0.0);                
                vec3 pt3 = pt_model_buffer->points[tri3-1];
                glVertex3f(pt3.x, pt3.y, pt3.z);

            }

        glEnd(); 
    }


    if (draw_quads )
    {
        
        glBindTexture(GL_TEXTURE_2D, texture[3]);

        glBegin(GL_QUADS);                      

            for (q_i=0;q_i<pt_model_buffer->num_quads;q_i++)
            { 

                int qu1 = pt_model_buffer->quads[q_i].pt1;
                int qu2 = pt_model_buffer->quads[q_i].pt2;
                int qu3 = pt_model_buffer->quads[q_i].pt3;
                int qu4 = pt_model_buffer->quads[q_i].pt4;


                glTexCoord2f(0.5, 1.0);                
                vec3 pt1 = pt_model_buffer->points[qu1-1];
                glVertex3f(pt1.x, pt1.y, pt1.z);

                //vec2 uv = pt_model_buffer->uvs[tri2];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.0, 1.0); 
                vec3 pt2 = pt_model_buffer->points[qu2-1];
                glVertex3f(pt2.x, pt2.y, pt2.z);

                glTexCoord2f(1.0, 0.0);                
                vec3 pt3 = pt_model_buffer->points[qu3-1];
                glVertex3f(pt3.x, pt3.y, pt3.z);

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


}//end resize callback

/***************************************/

void set_view_ortho(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                   
    gluOrtho2D(scr_size_x, scr_size_y, -1, 1 ); //(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
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

void olmec(int *argc, char** argv){
 
    glutInit(argc, argv);  

    //shader_test();
    set_colors();


    load_objfile(obj_filepath, pt_model_buffer ); 


    warnings();

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(scr_size_x, scr_size_y);  //window size
    glutInitWindowPosition(0, 0);  
    
    window_id = glutCreateWindow("Olmec v.00002.51"); //create an opengl window 

    /***********/
    reset_view();

    //register GL callbacks       

    glutDisplayFunc(&render_loop); 
    glutIdleFunc(&animate);

    glutReshapeFunc(&reshape_window);  // register window resize callback 

    glutKeyboardFunc(&keyPressed);     // register key pressed callback 

    InitGL(scr_size_x, scr_size_y); // Initialize window. 
  
    glutMouseFunc (olmec_mouse_button);
    glutMotionFunc (olmec_mouse_motion);




    loadImage("textures/generated2.bmp" , imageloaded_bfr);
    loadImage("textures/generated2.bmp" , imageloaded_bfr2);


    //copyBuffer24( imageloaded_bfr, loaded_texture ); //convert "RGBType" to "Image"

    // create and apply 2D texture   
    glGenTextures(4, &texture[0]);            //create 3 textures

    //grid color
    glBindTexture(GL_TEXTURE_2D, texture[0]);  
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); // scale linearly when image bigger than texture
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); // scale linearly when image smalled than texture
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

    glutMainLoop();// Start Event Processing Engine   


}






/**************************************************/
/**************************************************/


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
                   //printf("# orbit dist %f \n", orbit_dist );
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
            if(mouseLeftDown)
            {        
                view_ismoving = TRUE;
                
                orbit_x += (x-mouseX) * coefficient; 
                orbit_y += (y-mouseY) * coefficient; 
                mouseX = x;
                mouseY = y;

                //printf("# mouse motion %d %d %f %f \n", x,y, orbit_x, orbit_y );
            }

            if(mouseRightDown)
            {
                orbit_dist -= (y - mouseY) * 0.02f;
                mouseY = y;
            }

        break;

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
    

    if (key == 116) // t key 
    { 


        // if (render_text == TRUE){
        //     render_text = FALSE;
        // }else{
        //     render_text = TRUE;
        // }

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

            draw_points = FALSE;
        }else{

            draw_points = TRUE;
        }

    }

    if (key == 52) //4 - display as wire 
    { 
        glDisable(GL_TEXTURE_2D);        
        glDisable(GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK,  GL_LINE);

    }

    if (key == 53) //5 - display as solid, no texture  
    { 
        // glShadeModel(GL_SMOOTH);              

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }
    
    if (key == 54) //6 - display as solid, with texture 
    { 

        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }

    if (key == 55) //7
    { 

        glEnable(GL_LIGHTING);
        glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
    }

    //------

    if (key == 45) //minus
    { 
        orbit_dist-=.1;  
    }
    
    if (key == 61) //plus
    { 
        orbit_dist+=.1;  
    }

     //------

    if (key == 32) //space
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
        //show_obj_geom(pt_model_buffer);
    }


    if (key == 105) //i - draw bbox
    { 

        if (draw_bbox == TRUE){
            draw_bbox = FALSE;
        }else{
            draw_bbox = TRUE;
            show(pt_model_buffer);            
        }



    }

    if (key == 111) //o
    { 

        char* file2 = "3d_obj/PYCORE.obj";

        reset_objfile(pt_model_buffer, pt_obinfo);

        load_objfile(file2, pt_model_buffer );
        get_obj_info( pt_model_buffer, pt_obinfo);

    }

    if (key == 112) //p
    { 
        init_pycore(); 
    }


    if (key == 8 || key == 127) //backspace /delete on OSX )
    { 
        software_render();
    }

    //------
    if (key == 82) //shift r
    { 
        reset_objfile(pt_loader      , pt_obinfo);
        reset_objfile(pt_model_buffer, pt_obinfo);
    }

    if (key == 114) //r
    { 
        load_objfile(obj_filepath, pt_model_buffer ); 
        get_obj_info( pt_model_buffer, pt_obinfo);

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
    
    set_screen_square(&scr_size_x, &scr_size_y);

    //char* render_cmd = "runcommand";    
    char buffer[128];

    printf("./renderthing %d %d %s %d %d %d %s %d %d", scr_size_x, scr_size_y, obj_filepath, 0, 0, 0 ,"foo.bmp\n", (int)abs(orbit_dist*30) , 100);

    //                                                    xres yres inputfile X Y Z outputfile renderscale which
    snprintf(buffer, sizeof(buffer), "./renderthing %d %d %s %d %d %d %s %d %d", 512, 512, obj_filepath, 0, 0, 0 ,"foo.bmp", (int)abs(orbit_dist*30) , 100);
    int ret = system(buffer);

}



void init_pycore(void){
    //call python from here!!

    char* pycore_cmd = "runcommand";    
    char buffer[128];

    snprintf(buffer, sizeof(buffer), "python3 pycore.py %s %s", obj_filepath, pycore_cmd);
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

