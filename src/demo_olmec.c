
/*************************************************************/
/*
   demo_olmec.c 
     
      Named Olmec - because its not Maya :) 
      
      make a 3D environment to view models and play in that is navigatable in the way Maya does it


       -------------------------------------------------------------------------------------
       Maya coordinate system    |  Blender coordinate system  |    Fusion 360 Coordinate System
                                 |                             |  
            Y                    |        Z                    |  
            |                    |        |                    |      
            |                    |        |                    | 
            ______X (look on Z)  |        ____X (look on Y)    |
       -------------------------------------------------------------------------------------

 

    TODO:

        - LOAD MULTIPLE OBJECTS IN SCENE 
        - TEXTURES, LINE COLORS
        - LIGHTING 
        - OBJ SAVE 
        - WIDGETS, BUTTONS
        - SCENE FORMAT ?
        - ON SCREEN TEXT 
        - HANDLE PYCORE ERRORS 



      Copyright (C) 2019 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>      
#include <cmath>
#include "gl_setup.h"
#include "bitmap_io.h" // includes framebuffer.h, colors, etc 
#include "point_op.h"
#include "obj_model.h"
#include "demo_olmec.h"


/***********/
// window related 
extern int window_id;
extern int scr_size_x;
extern int scr_size_y;
extern bool scr_full_toglr;

// view prefs 
bool draw_lines     = TRUE;
bool draw_quads     = TRUE;
bool draw_triangles = TRUE;
bool draw_grid      = TRUE;

/***********/
// object related 
extern GLuint texture[1];
extern char* obj_filepath;

struct obj_model loader;
struct obj_model *pt_loader = &loader;

float xrot, yrot, zrot; 

/***********/
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



float ticker = 0;

/***************************************/
// camera properties 


// static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
// static int g_yClick = 0;
// float total_orbitx;

bool view_ismoving = FALSE;

float orbit_x  = 0;     // 2d click to set 3d view 
float orbit_y  = 0;   
extern float gui_zoomz; //Z zoom 

float cam_rotx = 0; // camera rotation
float cam_roty = 0;
float cam_rotz = 0;

float cam_posx = 0; // camera location
float cam_posy = 0;
float cam_posz = 0;


/***************************************/

void set_colors(void){
    //make a color for some lines 
    pt_linecolor->r = 255;
    pt_linecolor->g = 0;
    pt_linecolor->b = 0;

    //make a color for some lines 
    pt_linecolor2->r = 25;
    pt_linecolor2->g = 25;
    pt_linecolor2->b = 155;

    //make a color for some lines 
    pt_gridcolor->r = 200;
    pt_gridcolor->g = 200;
    pt_gridcolor->b = 200;

}


/***************************************/

void warnings(void)
{
    // let us know if there is a discernable problem 
    printf("\n\n\n\n###########################################\n");

    if(!draw_lines || !pt_loader->num_lines){
        printf("#warn - no lines or disabled.     \n");
    }

    if(!draw_triangles || !pt_loader->num_tris){
        printf("#warn - no triangles or disabled. \n");
    }

    if(!draw_quads || !pt_loader->num_quads){ ; }    

}

/***************************************/

void reset_view(void){
 
    orbit_x   = .125; //2d click to set 3d view 
    orbit_y   = -.06;   
    gui_zoomz = -1.0;  

    cam_rotx = 0; //camera rotation
    cam_roty = 0;
    cam_rotz = 0;

    cam_posx = .5; //camera location
    cam_posy = 1.5;
    cam_posz = .5;

}

/***************************************/
static void redraw_textures(Image *loaded_texture)
{
    // //test of new framebuffer commands
    // RGBType* pt_rgb_bfr =  createBuffer24(loaded_texture->sizeX, loaded_texture->sizeY);    
    // //copyBuffer24( loaded_texture ,  pt_rgb_bfr ); //convert "Image" to "RGBType"
    // copyBuffer24( imageloaded_bfr2 ,  pt_rgb_bfr ); //convert "Image" to "RGBType"
    // free(pt_rgb_bfr);

    copyBuffer24( imageloaded_bfr, loaded_texture ); //convert "RGBType" to "Image"

    // create and apply 2D texture   
    glGenTextures(3, &texture[0]);            //create 3 textures
    glBindTexture(GL_TEXTURE_2D, texture[0]); // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, loaded_texture->sizeX, loaded_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, loaded_texture->data);

}

/***************************************/

//draw a 3D grid on the "floor" and an indicator to show XYZ axis  
static void graticulate( void )
{
 
    int grd_num       = 10;
    
    float grd_height  = 0.0;
    float grd_size    = 2.5;
    float gspac = grd_size/(grd_num/2);

    glBindTexture(GL_TEXTURE_2D, texture[2]);   // choose the texture to use.

    float id = 0;

    //"swim bag" tile effect
    // for(id=0; id<=grd_size; id+=gspac)
    // {
    //     glVertex3f(-id, 0,  grd_size);
    //     glVertex3f(-id, 0, -grd_size);  
    //     glVertex3f(-id, 0, -id);
    //     glVertex3f( id, 0, -id);                
    // }

    glBegin(GL_LINES);
  
        for(id=-gspac; id<=grd_size; id+=gspac)
        {
            if(id==0)
            {  
                glColor3f(pt_linecolor2->r, pt_linecolor2->g, pt_linecolor2->b);                 
                glVertex3f( id, grd_height,   (grd_size+1) );

                glColor3f(pt_linecolor2->r, pt_linecolor2->g, pt_linecolor2->b);                 
                glVertex3f( id, grd_height,  -(grd_size+1) );  

                glColor3f(pt_linecolor2->r, pt_linecolor2->g, pt_linecolor2->b);   
                glVertex3f(  (grd_size+1), grd_height, id );

                glColor3f(pt_linecolor2->r, pt_linecolor2->g, pt_linecolor2->b);   
                glVertex3f( -(grd_size+1), grd_height, id ); 

            }else if (draw_grid) {
               
                glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);  
                glVertex3f(-id, grd_height,  grd_size);

                glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);                 
                glVertex3f(-id, grd_height, -grd_size);  
 
                glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);  
                glVertex3f( id, grd_height,  grd_size);

                glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);                  
                glVertex3f( id, grd_height, -grd_size); 

                glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);  
                glVertex3f(-grd_size, grd_height, -id);
                
                glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);  
                glVertex3f( grd_size, grd_height, -id);                
                
                glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);  
                glVertex3f(-grd_size, grd_height,  id);

                glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);                  
                glVertex3f( grd_size, grd_height,  id);                 
            }

        }
    glEnd();
}

/***************************************/
/*
void set_camera(void){

    // https://stackoverflow.com/questions/5717654/glulookat-explanation 
    // https://learnopengl.com/Getting-started/Camera

    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glRotate.xml
    // glGet with argument GL_MATRIX_MODE
    // glGet with argument GL_COLOR_MATRIX
    // glGet with argument GL_MODELVIEW_MATRIX
    // glGet with argument GL_PROJECTION_MATRIX
    // glGet with argument GL_TEXTURE_MATRIX

    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();               // Reset The View

    // view = new_m44();
    // gluLookAt( cam_x , 0.0, cam_z ,   // look from camera XYZ  
    //             0.0  , 0.0, 0.0  ,    // look at the origin 
    //             0.0  , 1.0, 0.0  );   // positive Y up vector    

    // gluLookAt(camera[0], camera[1], camera[2],  // look from camera XYZ  
    //                   0,         0,         0,  // look at the origin  
    //                   0,         1,         0); // positive Y up vector  
    
    //printf("# orbit %f \n", orbit_x );

    //glRotatef(orbit_x, 0.f, 1.0f, 0.f);             
    //glCallList(SCENE); // draw the scene 

}
*/



static void render_loop()
{

    // if (view_ismoving){
    //     //total_orbitx += orbit_x;
    //     printf("view difference is  %f \n" , orbit_x );
    //     printf("last difference was %f \n" , total_orbitx );
    // }


    // this is an absolute world move 
    //  it will never work right
    // you need to do move relative to the view and transform that to world 
    cam_posx = sin( orbit_x*5 ) * gui_zoomz;
    cam_posy = (-orbit_y*5);
    cam_posz = cos( orbit_x*5 ) * gui_zoomz;


    //printf("render loop %f %f %f %f %f %f \n", orbit_x, orbit_y, gui_zoomz, cam_posx, cam_posy, cam_posz );
    /******************************************/
    // Clear The Screen And The Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     

    /******************************************/
    //process geometry GL_PROJECTION

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();           
    
    gluLookAt( cam_posx, cam_posy , cam_posz,  // look from camera XYZ
                 0.0, 0.0   , 0.0,    // look at the origin
                 0.0, 1.0   , 0.0     // positive Y up vector
             ); 

    graticulate();
    
    
    // glRotatef( xrot , 1.0f, 0.0f, 0.0f);     // Rotate On The X Axis
    // glRotatef( yrot , 0.0f, 1.0f, 0.0f);     // Rotate On The Y Axis
    // glRotatef( zrot , 0.0f, 0.0f, 1.0f);     // Rotate On The Z Axis

    int q_i, p_i, f_i = 0;

    /******************************************/
    // draw 3D line geometry 
    if (draw_lines)
    {
        glBindTexture(GL_TEXTURE_2D, texture[1]);   // choose the texture to use.
            
        for (p_i=0;p_i<pt_loader->num_lines;p_i++)
        {   
            glBegin(GL_LINES);
                // fetch the line indices from vertex list 
                int lin1 = pt_loader->lines[p_i].pt1;
                int lin2 = pt_loader->lines[p_i].pt2;
                
                vec3 pt1 = pt_loader->points[lin1-1];
                vec3 pt2 = pt_loader->points[lin2-1];

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
        glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.

        glBegin(GL_TRIANGLES);  
            for (p_i=0;p_i<pt_loader->num_tris;p_i++)
            { 
                // fetch the triangle indices from vertex list
                int tri1 = pt_loader->tris[p_i].pt1;
                int tri2 = pt_loader->tris[p_i].pt2;
                int tri3 = pt_loader->tris[p_i].pt3;

                
                //vec2 uv = pt_loader->uvs[tri1];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.5, 1.0);                
                vec3 pt1 = pt_loader->points[tri1-1];
                glVertex3f(pt1.x, pt1.y, pt1.z);

                //vec2 uv = pt_loader->uvs[tri2];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.0, 1.0); 
                vec3 pt2 = pt_loader->points[tri2-1];
                glVertex3f(pt2.x, pt2.y, pt2.z);

                //vec2 uv = pt_loader->uvs[tri3];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(1.0, 0.0);                
                vec3 pt3 = pt_loader->points[tri3-1];
                glVertex3f(pt3.x, pt3.y, pt3.z);

            }

        glEnd(); 
    }


    if (draw_quads )
    {
        glBegin(GL_QUADS);                      

            for (q_i=0;q_i<pt_loader->num_quads;q_i++)
            { 

                int qu1 = pt_loader->quads[q_i].pt1;
                int qu2 = pt_loader->quads[q_i].pt2;
                int qu3 = pt_loader->quads[q_i].pt3;
                int qu4 = pt_loader->quads[q_i].pt4;


                glTexCoord2f(0.5, 1.0);                
                vec3 pt1 = pt_loader->points[qu1-1];
                glVertex3f(pt1.x, pt1.y, pt1.z);

                //vec2 uv = pt_loader->uvs[tri2];
                //glTexCoord2f(uv.x, uv.y);
                glTexCoord2f(0.0, 1.0); 
                vec3 pt2 = pt_loader->points[qu2-1];
                glVertex3f(pt2.x, pt2.y, pt2.z);

                glTexCoord2f(1.0, 0.0);                
                vec3 pt3 = pt_loader->points[qu3-1];
                glVertex3f(pt3.x, pt3.y, pt3.z);

                glTexCoord2f(1.0, 0.0);                
                vec3 pt4 = pt_loader->points[qu4-1];
                glVertex3f(pt4.x, pt4.y, pt4.z);


            }
        

        glEnd(); 
    }


    //redraw_textures(main_bg_bfr);

    // swap the other (double) buffer to display what just got drawn.
    glutSwapBuffers();
  
    view_ismoving = FALSE;
}

/***************************************/
void animate(){
    ticker += .01;
    render_loop();
}
/***************************************/

static void reshape_window(int width, int height)
{
    
    if ( height == 0 )                // Prevent A Divide By Zero If The Window Is Too Small
        height=1;
    glViewport(0, 0, width, height);  // Reset The Current Viewport And Perspective Transformation

}//end resize callback

/***************************************/

//define keyboard events 
static void keyPressed(unsigned char key, int x, int y) 
{

    //printf("key %u \n", key );

    usleep(100);

    //ESCAPE KEY
    if (key == 27) 
    { 
        glutDestroyWindow(window_id); 
        exit(0);                   
    }
    

    /*
        //MAYA keys 
        // 1 - persp 
        // 2 - front (pos Z)
        // 3 - side  

        // 4 wireframe 
        // 5 shaded 
        // 6 shaded textured 
        // 7 use all lights 
        // q - select tool 
        // a - frame all objects 
        // w - move 
        // e - rotate 
        // r - scale 
        // f - frame selected 
        // t - show manipulator 
        // p - parent 
        // shft p - unparent 


    */
    


    if (key == 45) //minus
    { 
        gui_zoomz--;  
    }
    
    if (key == 61) //plus
    { 
        gui_zoomz++;  
    }

    // printf("scancode is  %d \n", key );

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
    



    if (key == 111) //o
    { 
        // reset_objfile(pt_loader);
        char* file2 = "3d_obj/PYCORE.obj";
        load_objfile(file2, pt_loader );

    }

    if (key == 112) //p
    { 
        init_pycore(); 
    }

    if (key == 114) //r
    { 
        // reset_objfile(pt_loader);
        load_objfile(obj_filepath, pt_loader ); 
    }

    if (key == 102) //f
    { 
        reset_view();
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


/********************************************/

void olmec_mouse_button(int button, int state, int x, int y)
{
    // Respond to mouse button presses.
    // If button1 pressed, mark this state so we know in motion function.

    // printf("olmec button x %d y %d \n",x , y);

    //left click 
    if (button == GLUT_LEFT_BUTTON)
      {
        //g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
        //g_yClick = y - 3 * g_fViewDistance;
        
        // printf("olmec left click \n");

      }

      // middle click
      if ((button == 3) || (button == 4)) // It's a wheel event
      {
           // Disregard redundant GLUT_UP events
           if (state == GLUT_UP) return; 

           if (button == 3){
               if (gui_zoomz < -1.5){
                   gui_zoomz+=1;                
               }
  
           }
           if (button == 4){
               gui_zoomz--; 
           }
      }else{  // normal button event
           if (state == GLUT_DOWN){
               // printf("olmec middle click\n");  
           }
      }


    //Right click
    if (button == GLUT_RIGHT_BUTTON)
      {
        
        // printf("olmec right click \n");

      }

}



/********************************************/

void olmec_mouse_motion(int x, int y)
{
    // If button1 pressed, zoom in/out if mouse is moved up/down.
    view_ismoving = TRUE;

    //take offset from center of screen to get "X,Y delta"
    float center_x = (float)scr_size_x/2;
    orbit_x = (center_x-x)/scr_size_x; 

    float center_y = (float)scr_size_y/2;
    orbit_y = (center_y-y)/scr_size_y; 

    // printf("# orbit %f \n", orbit_y);

    // if (g_bButton1Down)
    // {
    //     // printf("MOVE! %f %d \n", cube_x, y);
    //     
    //     // g_fViewDistance = (y - g_yClick) / 3.0;
    //     // if (g_fViewDistance < VIEWING_DISTANCE_MIN)
    //     //    g_fViewDistance = VIEWING_DISTANCE_MIN;
    //     // glutPostRedisplay();
    // }


}


/********************************************/
/********************************************/

//attempt to port code from Unity Engine into pure C 

float zoomSpeed    = 1.2f;
float moveSpeed    = 1.9f;
float rotateSpeed  = 4.0f;
vec3 startpos = newvec3(0.0, 130.0, 60.0);

m33 capsuleObj; //represents a Unity/Maya Transform node 
quaternion orbt_rot_original;
vec3 orbt_xform_original;

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


/********************************************/
/********************************************/

void olmec(int *argc, char** argv){
 
    glutInit(argc, argv);  

    //shader_test();
    set_colors();


    load_objfile(obj_filepath, pt_loader ); 

    warnings();

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(scr_size_x, scr_size_y);  //window size
    glutInitWindowPosition(0, 0);  
    
    window_id = glutCreateWindow("Olmec v.000001"); //create an opengl window 

    /***********/
    //THERE IS A MEMORY LEAK, PROBABLY IN THIS FUNCTION!!
    //TO TEST RUN AND LEAVE WINDOW FOR A WHILE 
    //GO LEARN VALGRIND AND FIX IT ?

    reset_view();

    //register GL callbacks       
    glutDisplayFunc(&render_loop);   
    glutIdleFunc(&animate);
    glutReshapeFunc(&reshape_window);  // register window resize callback 

    glutKeyboardFunc(&keyPressed);     // register key pressed callback 

    InitGL(scr_size_x, scr_size_y); // Initialize window. 
  
    glutMouseFunc (olmec_mouse_button);
    glutMotionFunc (olmec_mouse_motion);

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

    loadImage("textures/generated1.bmp" , imageloaded_bfr2);
    loadImage("textures/generated3.bmp" , imageloaded_bfr);

    redraw_textures(main_bg_bfr);

    glutMainLoop();// Start Event Processing Engine   


}





/***********/

void software_render(void){
    //#include <sys/syscall.h>  //experiment to call renderer 

    /* test of a system call to an external program 
       one day I may attempt to implement a seperate software renderer here  
    */

    // syscall(SYS_write, 1, "hello, world!\n", 14);  //experiment to call render program 
    // printf("%d\n", SYS_write);

    int ret = system("ls");
    // if (WIFSIGNALED(ret) &&
    //     (WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT)){            
    //     break;
    // }

}



void init_pycore(void){
    //call python from here!!

    char* pycore_cmd = "runcommand";    
    char buffer[128];

    snprintf(buffer, sizeof(buffer), "python3 pycore.py %s %s", obj_filepath, pycore_cmd);
    int ret = system(buffer);

    //load the result in !!
    //char* newfilepath = "3d_obj/normals.obj";
    //load_objfile( newfilepath, pt_loader ); 
}

