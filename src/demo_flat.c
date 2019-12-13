/*************************************************************/
/*
   demo_flat.c 



   Copyright (C) 2019 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>      

#include "demo_flat.h"

#include "bitmap_io.h"
#include "gl_setup.h"


extern int window_id;

extern int use_tex;
extern GLuint texture[3];  

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


static bool g_bButton1Down = FALSE;
static bool g_bLightingEnabled = TRUE;
static bool g_bFillPolygons = TRUE;
static bool g_bTexture = FALSE;

static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
static int g_yClick = 0;


enum {
    MENU_ABOUT,
    MENU_POLYMODE,
    MENU_TEXTURING,
    MENU_EXIT
};

void SelectFromMenu(int idCommand)
{
  switch (idCommand)
    {
        case MENU_ABOUT:
          g_bLightingEnabled = !g_bLightingEnabled;
          if (g_bLightingEnabled)
             glEnable(GL_LIGHTING);
          else
             glDisable(GL_LIGHTING);
        break;

        case MENU_POLYMODE:
          g_bFillPolygons = !g_bFillPolygons;
          glPolygonMode (GL_FRONT_AND_BACK, g_bFillPolygons ? GL_FILL : GL_LINE);
        break;      

        case MENU_TEXTURING:
          g_bTexture = !g_bTexture;
          if (g_bTexture)
             glEnable(GL_TEXTURE_2D);
          else
             glDisable(GL_TEXTURE_2D);
        break;    

        case MENU_EXIT:
          exit (0);
        break;
    }


    glutPostRedisplay();
}


void animateTextures2(Image *loaded_texture)
{

    RGBType bgcolor;
    RGBType *pt_bgcolor = &bgcolor;
    pt_bgcolor->r = 45;
    pt_bgcolor->g = 50;
    pt_bgcolor->b = 50;

    RGBType linecolor;
    RGBType *pt_linecolor = &linecolor;
    pt_linecolor->g = 230;
    pt_linecolor->b = 130;
    
    RGBType bordercolor;
    RGBType *pt_bordercolor = &bordercolor;
    pt_bordercolor->b = 255;


    RGBType hitcolor;
    RGBType *pt_hitcolor = &hitcolor;
    pt_hitcolor->r = 200;
    pt_hitcolor->b = 128;
  
    RGBType firecolor;
    RGBType *pt_firecolor = &firecolor;
    pt_firecolor->r = 227;
    pt_firecolor->g = 171;
    pt_firecolor->b = 16;    


    int cp_tl[2] = {0};
    int cp_br[2] = {0};
    
    cp_tl[0] = upos-pong_size;
    cp_tl[1] = vpos-pong_size;
    cp_br[0] = upos+pong_size;
    cp_br[1] = vpos+pong_size;
   
    //clear background image 
    fillbuffer24(loaded_texture, pt_bgcolor);
    
    // draw background image 
    if (use_tex==0){ copyBuffer24( imageloaded_bfr  , loaded_texture );}
    if (use_tex==1){ copyBuffer24( imageloaded_bfr2 , loaded_texture );}

     
    //draw the pong bouncer 
    if(upos>(loaded_texture->sizeX-(pong_size*2)))
    {
        draw_fill_square(  loaded_texture,  upos, vpos, pong_size*1.2, pt_hitcolor  );
        copyBuffer24( imageloaded_bfr , loaded_texture, cp_tl, cp_br );
        //draw_fill_square(  loaded_texture,  upos, vpos, pong_size, pt_firecolor  );
    }else{
        draw_fill_square(  loaded_texture,  upos, vpos, pong_size, pt_linecolor  );
    }

    ////

    //origin is bottom left - grr 
    int tl[2] = { 0, 0 };
    int br[2] = { loaded_texture->sizeX-1, loaded_texture->sizeY-1 };
    draw_square( loaded_texture, loaded_texture->sizeX, tl, br, pt_bordercolor );
   



    // Create Texture   
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    glTexImage2D(GL_TEXTURE_2D, 0, 3, loaded_texture->sizeX, loaded_texture->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, loaded_texture->data);
    
    //free(loaded_texture->data);
}


/***************************************/
/*
    cobbled together display callback to create a quad polygon with a UV map 
*/
 
// blitBuffer32

void drawglscene_2d()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // Clear The Screen And The Depth Buffer
    glLoadIdentity(); // Reset The View
    glTranslatef(0.0f, 0.0f, -3.7f);    // move 5 units into the screen.
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.

    //define geometry 
    glBegin(GL_QUADS);//draw a four sided polygon(s)
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);  // BL texture and quad
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);  // BR texture and quad
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);  // TR texture and quad
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);  // TL texture and quad
    glEnd(); // end polygon.
    
    /*
    glBegin(GL_POINTS);
        glVertex2f(0.0, 0.0);
        glVertex2f(1.0, 0.0);
        glVertex2f(0.0, 1.0);
        glVertex2f(1.0, 1.0);
        glVertex2f(0.5, 0.5);
    glEnd();
    */

    /*
    glBegin(GL_LINES);
        glVertex2f(0.0, 0.0);
        glVertex2f(1.0, 0.0);
    glEnd();
    */

    unsigned int u_edge = img_usize - pong_size;
    unsigned int v_edge = img_vsize - pong_size;

    // animate some numbers 
    if(upos>=u_edge)    { dir_u = 1; }
    if(upos<=pong_size) { dir_u = 0; }
    if(vpos>=u_edge)    { dir_v = 1; }
    if(vpos<=pong_size) { dir_v = 0; }
    if(dir_u==0)        { upos +=pong_speed;}else{ upos -=pong_speed;}
    if(dir_v==0)        { vpos +=pong_speed;}else{ vpos -=pong_speed;}
  
    animateTextures2(main_bg_bfr);

    // since this is double buffered, swap the buffers to display what just got drawn.
    glutSwapBuffers();
    
}//end display callback 



/*****************************************/

int BuildPopupMenu (void)
{
    int menu;

    menu = glutCreateMenu (SelectFromMenu);
    glutAddMenuEntry ("About semraster\tl", MENU_ABOUT);
    glutAddMenuEntry ("Toggle polygon fill\tp", MENU_POLYMODE);
    glutAddMenuEntry ("Toggle texturing\tt", MENU_TEXTURING);
    glutAddMenuEntry ("Exit  ", MENU_EXIT);

    return menu;
}

/*****************************************/
void MouseButton(int button, int state, int x, int y)
{
    // Respond to mouse button presses.
    // If button1 pressed, mark this state so we know in motion function.

    if (button == GLUT_LEFT_BUTTON)
    {
        g_bButton1Down = (state == GLUT_DOWN) ? TRUE : FALSE;
        g_yClick = y - 3 * g_fViewDistance;
    }

}

/*****************************************/
void MouseMotion(int x, int y)
{
    // If button1 pressed, zoom in/out if mouse is moved up/down.

    if (g_bButton1Down)
    {
        
        // g_fViewDistance = (y - g_yClick) / 3.0;
        // if (g_fViewDistance < VIEWING_DISTANCE_MIN)
        //    g_fViewDistance = VIEWING_DISTANCE_MIN;
        // glutPostRedisplay();
    }
}

/*****************************************/
// callback when window is resized (which shouldn't happen in fullscreen) 
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
        if (pong_size<200){
            pong_size +=1;
        }      
    }

    if (key == 100) //d
    { 
 
        if (pong_size>4){
            pong_size -=1;
        }

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


/*****************************************/
// top level opengl loop 
void flatImageDemo(int *argc, char** argv){
    
    int screenSize = 512; //defaults to 512
    if (argv[1]){
       screenSize = atoi(argv[1]);
    }

    printf("\n\nstarting up semraster in %i resolution.\n", screenSize);

    // you can find documentation at http://reality.sgi.com/mjk/spec3/spec3.html   
    glutInit(argc, argv);  

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH);  
    glutInitWindowSize(screenSize, screenSize);  //window size

    // the window starts at the upper left corner of the screen  
    glutInitWindowPosition(0, 0);  
     
    window_id = glutCreateWindow("2D polygon demo"); //create an opengl window 

    glutDisplayFunc(&drawglscene_2d);//register display callback       

    // Even if there are no events, redraw our gl scene.  
    glutIdleFunc(&drawglscene_2d);

    glutReshapeFunc(&ReSizeGLScene);  //register window resize callback 
    glutKeyboardFunc(&keyPressed);    // Register key pressed callback 

    InitGL(screenSize, screenSize);   // Initialize window. 
    
    ///////////////////////////
    //test of BMP saving 
    //create_Image("generated1.bmp");  
    //create_Image2("generated2.bmp"); 
    
    glutMouseFunc (MouseButton);
    glutMotionFunc (MouseMotion);
  
    // Create our popup menu
    BuildPopupMenu ();
    glutAttachMenu (GLUT_RIGHT_BUTTON);
    
    /***********************/
    // 2d playground for framebuffer 
      
    loadImage("textures/generated1.bmp", imageloaded_bfr);
    loadImage("textures/generated3.bmp", imageloaded_bfr2);
   
    /*   
    //test of new framebuffer commands
    RGBType* pt_rgb_bfr =  createBuffer24(imageloaded_bfr->sizeX, imageloaded_bfr->sizeY);    
    
    //copyBuffer24( imageloaded_bfr2 ,  pt_rgb_bfr ); //convert "Image" to "RGBType"

    //make a color for some lines 
    RGBType line_color;
    RGBType *pt_linecolor = &line_color;
    pt_linecolor->r = 255;
    pt_linecolor->g = 0;
    pt_linecolor->b = 0;

    // draw 4 lines - crisscross applesauce 
    //draw_line(pt_rgb_bfr, imageloaded_bfr->sizeX, 0  , 20  , 50  , 50  , pt_linecolor); 
    draw_line(pt_rgb_bfr, imageloaded_bfr->sizeX, 0  , 0   , 511 , 511 , pt_linecolor); 
    draw_line(pt_rgb_bfr, imageloaded_bfr->sizeX, 511, 0   , 0   , 511 , pt_linecolor); 
    //draw_line(pt_rgb_bfr, imageloaded_bfr->sizeX, 0  , 511 , 511 , 0   , pt_linecolor);

    // const char *filename = "rgb_buffer.bmp";
    // saveBMP_24bit (pt_rgb_bfr, filename , image_x, image_y);
    // free(pt_image_bfr);

    //overwrite loaded buffer with lines
    copyBuffer24( pt_rgb_bfr, imageloaded_bfr ); //convert "RGBType" to "Image"
    */
    
    /***********************/

    glutMainLoop();// Start Event Processing Engine   
   
}