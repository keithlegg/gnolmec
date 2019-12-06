/*************************************************************/
/*
   demo_olmec.c 
     
      Named Olmec - because its not Maya :) 
      
      make a 3D environment that is navigatable in the way Maya3D does it
      (alt-click rotate view, etc)


   Copyright (C) 2019 Keith Legg (keithlegg23@gmail.com)

*/
/*************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>      

#include "gl_setup.h"
#include "bitmap_io.h" 

#include "obj_model.h"

#include "demo_olmec.h"


extern float xrot, yrot, zrot; 

extern int window_id;

extern int use_tex;
extern GLuint texture[1];  

extern unsigned int pong_size;
extern unsigned int img_usize;
extern unsigned int img_vsize;
extern unsigned int upos  ; 
extern unsigned int vpos  ;   
extern unsigned int dir_u ; 
extern unsigned int dir_v ; 
extern unsigned int pong_speed;

extern Image* main_bg_bfr      ; 
extern Image* imageloaded_bfr2 ; 
extern Image* imageloaded_bfr  ; 


static BOOL g_bButton1Down = FALSE;
static BOOL g_bLightingEnabled = TRUE;
static BOOL g_bFillPolygons = TRUE;
static BOOL g_bTexture = FALSE;

static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
static int g_yClick = 0;


extern float gui_rotx;
extern float gui_roty;
extern float gui_zoomz;

extern int scr_size_x;
extern int scr_size_y;

struct obj_model loader;
struct obj_model *pt_loader = &loader;



extern char* obj_filepath;


void system_render(void){
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


void reset_view(void){
    gui_rotx = 0.0;
    gui_roty = 0.0;
    gui_zoomz = -5.0;  
}

static void animateTextures3(Image *loaded_texture)
{
    //test of new framebuffer commands
    RGBType* pt_rgb_bfr =  createBuffer24(loaded_texture->sizeX, loaded_texture->sizeY);    
    
    //copyBuffer24( loaded_texture ,  pt_rgb_bfr ); //convert "Image" to "RGBType"
    
    copyBuffer24( imageloaded_bfr2 ,  pt_rgb_bfr ); //convert "Image" to "RGBType"

    // 

    //make a color for some lines 
    RGBType line_color;
    RGBType *pt_linecolor = &line_color;
    pt_linecolor->r = 255;
    pt_linecolor->g = 0;
    pt_linecolor->b = 0;

    //make a color for some lines 
    RGBType line_color2;
    RGBType *pt_linecolor2 = &line_color2;
    pt_linecolor2->r = 155;
    pt_linecolor2->g = 255;
    pt_linecolor2->b = 0;

    // int cp_tl[2] = {0};
    // int cp_br[2] = {0};
    // cp_tl[0] = upos-pong_size;
    // cp_tl[1] = vpos-pong_size;
    // cp_br[0] = upos+pong_size;
    // cp_br[1] = vpos+pong_size;
    // draw_line(pt_rgb_bfr, loaded_texture->sizeX , cp_tl[0] , cp_tl[1] , cp_br[0] , cp_br[1] , pt_linecolor); 
    // draw_line(pt_rgb_bfr, loaded_texture->sizeX , cp_tl[0] , cp_tl[1] , cp_tl[0] , cp_br[1] , pt_linecolor);
    
    //just to show its alive - bounce a ball around 
    //draw_fill_circle ( pt_rgb_bfr, loaded_texture->sizeX, upos, vpos, pong_size, pt_linecolor);

    // const char *filename = "rgb_buffer.bmp";
    // saveBMP_24bit (pt_rgb_bfr, filename , image_x, image_y);
    // free(pt_image_bfr);

    ///////////////////////
    /*
    int x = 0;
    
    int num_cells = 20;
    int cell_size = (int)loaded_texture->sizeX/num_cells;
    
    for (x=cell_size;x<=loaded_texture->sizeX-cell_size;x=x+cell_size){
        draw_fill_circle ( pt_rgb_bfr, loaded_texture->sizeX, x, vpos, cell_size/2, pt_linecolor2);
    }
    */

    ///////////////////////
    /*
    // first stab at some matrix and vector art 
    m33 identity = new_m33(1,0,0,
                           0,1,0,
                           0,0,1);

    vec2 v1 = newvec2(102.0, 301.0);

    draw_line(pt_rgb_bfr, loaded_texture->sizeX , v1.x , v1.y , 0 , 0 , pt_linecolor);
    */
    ///////////////////////

    //overwrite loaded buffer with lines
    copyBuffer24( pt_rgb_bfr, loaded_texture ); //convert "RGBType" to "Image"

    free(pt_rgb_bfr);
     
    // create and apply 2D texture   
    glGenTextures(2, &texture[0]);  //create 2 textures

    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    glTexImage2D(GL_TEXTURE_2D, 0, 3, loaded_texture->sizeX, loaded_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, loaded_texture->data);

}

/***************************************/

static void display_loop()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear The Screen And The Depth Buffer
    glLoadIdentity();               // Reset The View

    double viewzoom = gui_zoomz;
    glTranslatef(0.0f,0, viewzoom);              // move 5 units into the screen.

    //rotate model based on view 
    xrot = gui_roty*-100;
    yrot = gui_rotx*-100;
    
    glRotatef( xrot , 1.0f, 0.0f, 0.0f);     // Rotate On The X Axis
    glRotatef( yrot , 0.0f, 1.0f, 0.0f);     // Rotate On The Y Axis
    glRotatef( zrot , 0.0f, 0.0f, 1.0f);     // Rotate On The Z Axis


    int p_i, f_i = 0;



    /********************************/
    // draw 3D line geometry 
    glBindTexture(GL_TEXTURE_2D, texture[1]);   // choose the texture to use.

    glBegin(GL_LINES);
        
        for (p_i=0;p_i<pt_loader->num_lines;p_i++)
        {   

            // fetch the line indices from vertex list 
            int lin1 = pt_loader->lines[p_i].pt1;
            int lin2 = pt_loader->lines[p_i].pt2;
            
            vec3 pt1 = pt_loader->points[lin1];
            vec3 pt2 = pt_loader->points[lin2];

            //print_vec3( pt1 );
            //print_vec3( pt2 );

            glColor3f(100.0, 0.0, 0.0);   
            //glVertex3f(pt1.x, pt1.y, pt1.z);
            glVertex2f(pt1.x, pt1.y);

            glColor3f(200.0, 200.0, 200.0); 
            //glVertex3f(pt1.x, pt1.y, pt1.z);
            glVertex2f(pt1.x, pt1.y);

        }

        // glVertex3f(0, 0, 0);    
        // glVertex3f(5, 5, 5);

    glEnd();

    /******************************************/
    // draw the polygon geometry 

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

    /******************************************/

    unsigned int u_edge = img_usize - pong_size;
    unsigned int v_edge = img_vsize - pong_size;

    if(upos<0)         { upos = 100; }
    if(vpos<0)         { vpos = 100; }
    if(upos>img_usize) { upos = 100; }
    if(vpos>img_vsize) { vpos = 100; }


    // animate some numbers 
    if(upos>=u_edge)     { dir_u = 1; }
    if(upos<=pong_size)  { dir_u = 0; }

    if(vpos>=u_edge)    { dir_v = 1; }
    if(vpos<=pong_size) { dir_v = 0; }

    if(dir_u==0)        { upos +=pong_speed;}else{ upos -=pong_speed;}
    if(dir_v==0)        { vpos +=pong_speed;}else{ vpos -=pong_speed;}

    
    //animateTextures2(main_bg_bfr);
    
    animateTextures3(main_bg_bfr);

    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
}


/***************************************/

static void ReSizeGLScene(int Width, int Height)
{
    
    if (Height==0)  // Prevent A Divide By Zero If The Window Is Too Small
    Height=1;

    glViewport(0, 0, Width, Height);    // Reset The Current Viewport And Perspective Transformation

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //gluOrtho2D(0, 100, 0, 100);
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);
    
    glMatrixMode(GL_MODELVIEW);
}//end resize callback

/***************************************/

//define keyboard events 
static void keyPressed(unsigned char key, int x, int y) 
{

    usleep(100);

    //ESCAPE KEY
    if (key == 27) 
    { 
        glutDestroyWindow(window_id); 
        exit(0);                   
    }
    
    // ALT is 38?

    if (key == 97) //a
    { 
        if (pong_speed<20){
            pong_speed +=1;
        }

    }
 
    if (key == 98) //b
    { 
        //printf("you pressed b\n");
        if (pong_speed>0){
            pong_speed -=1;
        }

    }
  
    if (key == 99) //c
    { 
        //test_loader_data(pt_loader); 
        //show_loader(pt_loader);


        //load_objfile(obj_filepath, pt_loader );   
    }

    if (key == 100) //d
    { 
 
    }

    if (key == 114) //r
    { 
        reset_view();
    }
        

    if (key == 101) //e
    { 
        glutReshapeWindow(800, 800);
        glutPositionWindow(0,0);
    }        

    if (key == 102) //f
    { 
        //printf("you pressed f\n");
        glutFullScreen();
    }

    if (key == 119) //w
    { 
        if(use_tex==0){
            use_tex=1;
        }
        else{
            use_tex=0;
        }  
    }

    if (key == 113) //q
    { 
        //printf("you pressed q\n");
        testLoadBinary();      
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
        g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
        g_yClick = y - 3 * g_fViewDistance;
        
        // printf("olmec left click \n");

      }

      // middle click
      if ((button == 3) || (button == 4)) // It's a wheel event
      {
           // Disregard redundant GLUT_UP events
           if (state == GLUT_UP) return; 

           if (button == 3){
               if (gui_zoomz < -1){
                   gui_zoomz++;                
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
 
    float center_x = (float)img_usize/2;
    gui_rotx = (center_x-x)/scr_size_x; 

    float center_y = (float)img_vsize/2;
    gui_roty = (center_y-y)/scr_size_y; 

    // cube_x

    if (g_bButton1Down)
    {
        // printf("MOVE! %f %d \n", cube_x, y);
        
        g_fViewDistance = (y - g_yClick) / 3.0;
        if (g_fViewDistance < VIEWING_DISTANCE_MIN)
           g_fViewDistance = VIEWING_DISTANCE_MIN;
        glutPostRedisplay();
    }


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


void olmecnav_rst_xforms( void ){
    //     transform.parent = capsuleObj.transform;
    //     capsuleObj.transform.position = orbt_xform_original ;
    //     capsuleObj.transform.rotation = orbt_rot_original;
    //     transform.parent = null;
    //     transform.position = startpos;
}




/********************************************/
/********************************************/

void olmec_navigation_demo(int *argc, char** argv){

    // you can find documentation at http://reality.sgi.com/mjk/spec3/spec3.html   
    glutInit(argc, argv);  


    //shader_test();
    
    load_objfile(obj_filepath, pt_loader ); 

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(scr_size_x, scr_size_y);  //window size

    // the window starts at the upper left corner of the screen  
    glutInitWindowPosition(0, 0);  
     
    window_id = glutCreateWindow("Olmec v.000001"); //create an opengl window 

    //register display callback       
    glutDisplayFunc(&display_loop);   

    // Even if there are no events, redraw our gl scene.  
    glutIdleFunc(&display_loop);

    glutReshapeFunc(&ReSizeGLScene);  //register window resize callback 
    glutKeyboardFunc(&keyPressed);    // Register key pressed callback 
    
    InitGL(scr_size_x, scr_size_y); // Initialize window. 


    ///////////////////////////    
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

    ///////////////////////////      
    // Create our popup menu
    //BuildPopupMenu ();
    //glutAttachMenu (GLUT_RIGHT_BUTTON);

    loadImage("textures/generated1.bmp" , imageloaded_bfr);
    loadImage("textures/generated3.bmp" , imageloaded_bfr2);

    glutMainLoop();// Start Event Processing Engine   


}


