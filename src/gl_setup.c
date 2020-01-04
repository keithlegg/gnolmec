/*************************************************************/
/*
   gl_setup.c 
       -  common home for an openGL application 
          used by the 3 built in demos (maya, flat and cube)
          
   Copyright (C) 2019 Keith Legg - keithlegg23@gmail.com

*/
/*************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "obj_model.h"
#include "framebuffer.h"
#include "gl_setup.h"


extern GLuint texture[3];


static GLfloat emis_full[]   = { 1, 1, 1, 0};
static GLfloat emis_off[]    = { 0, 0, 0, 0};
static GLfloat emis_half[]   = { .5, .5, .5, 0};
static GLfloat emis_teal[]   = { 0, 1., 1., 0};
static GLfloat emis_red[]    = { 1., 0, 0, 0};
static GLfloat emis_green[]  = { 0, 1., 0, 0};
static GLfloat emis_blue[]   = { 0, 0, 1., 0};

// quaternion frank = quaternion_identity();


/*************************************************************/


// m33_from_euler
// m44_from_euler

/*
   X - R
   Y - G
   Z - B 
*/

void render_m33(m33 *t33)
{
    // if transpose:
    //     v1 = vec3(m33[0], m33[3], m33[6])
    //     v2 = vec3(m33[1], m33[4], m33[7])
    //     v3 = vec3(m33[2], m33[5], m33[8])  

    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // Not transposed
    vec3 v1 = newvec3(t33->m0, t33->m1, t33->m2);
    vec3 v2 = newvec3(t33->m3, t33->m4, t33->m5);
    vec3 v3 = newvec3(t33->m6, t33->m7, t33->m8);

    glBegin(GL_LINES);
      
       glMaterialfv(GL_FRONT, GL_EMISSION, emis_red);
       glVertex3f (0   ,    0,    0);
       glVertex3f (v1.x, v1.y, v1.z);
      
       glMaterialfv(GL_FRONT, GL_EMISSION, emis_green);
       glVertex3f (0   ,    0,    0);
       glVertex3f (v2.x, v2.y, v2.z);

       glMaterialfv(GL_FRONT, GL_EMISSION, emis_blue);       
       glVertex3f (0   ,    0,    0);
       glVertex3f (v3.x, v3.y, v3.z);

        
    glEnd ();

    glMaterialfv(GL_FRONT, GL_EMISSION, emis_off); 
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 
}

/*************************************************************/

void render_m44(m44 *t44)
{
  
    // ⎡0   1   2   3 ⎤     ⎡xx  xy  xz  0⎤
    // ⎢4   5   6   7 ⎥     ⎢yx  yy  yz  0⎥
    // ⎢8   9   10  11⎥     ⎢zx  zy  zz  0⎥
    // ⎣12  13  14  15⎦     ⎣0   0   0   0⎦

    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    // Not transposed
    vec3 v1 = newvec3(t44->m0, t44->m1, t44->m2);
    vec3 v2 = newvec3(t44->m4, t44->m5, t44->m6);
    vec3 v3 = newvec3(t44->m8, t44->m9, t44->m10);

    glBegin(GL_LINES);
       glMaterialfv(GL_FRONT, GL_EMISSION, emis_red);    
       glVertex3f (0   ,    0,    0);
       glVertex3f (v1.x, v1.y, v1.z);

       glMaterialfv(GL_FRONT, GL_EMISSION, emis_green);       
       glVertex3f (0   ,    0,    0);
       glVertex3f (v2.x, v2.y, v2.z);

       glMaterialfv(GL_FRONT, GL_EMISSION, emis_blue);        
       glVertex3f (0   ,    0,    0);
       glVertex3f (v3.x, v3.y, v3.z);

        
    glEnd ();

    glMaterialfv(GL_FRONT, GL_EMISSION, emis_off); 
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full); 

}


/*************************************************************/
//void m44_to_glm44( m44* pt_m44, GLfloat m44_glfloat[16] ){}

void glutm44_to_m44( m44* pt_m44, GLfloat m44_glfloat[16] ){

    pt_m44->m0  = m44_glfloat[0];
    pt_m44->m1  = m44_glfloat[1];
    pt_m44->m2  = m44_glfloat[2];
    pt_m44->m3  = m44_glfloat[3];

    pt_m44->m4  = m44_glfloat[4];
    pt_m44->m5  = m44_glfloat[5];
    pt_m44->m6  = m44_glfloat[6];
    pt_m44->m7  = m44_glfloat[7];
    
    pt_m44->m8  = m44_glfloat[8];
    pt_m44->m9  = m44_glfloat[9];
    pt_m44->m10 = m44_glfloat[10];
    pt_m44->m11 = m44_glfloat[11];
    
    pt_m44->m12 = m44_glfloat[12];
    pt_m44->m13 = m44_glfloat[13];
    pt_m44->m14 = m44_glfloat[14];
    pt_m44->m15 = m44_glfloat[15];

} 



/*************************************************************/

void drawTriangle (GLfloat x1, GLfloat y1, GLfloat x2, 
    GLfloat y2, GLfloat x3, GLfloat y3, GLfloat z)
{
   glBegin (GL_TRIANGLES);
       glVertex3f (x1, y1, z);
       glVertex3f (x2, y2, z);
       glVertex3f (x3, y3, z);
   glEnd ();
}


/*************************************************************/

void drawViewVolume (GLfloat x1, GLfloat x2, GLfloat y1, 
                     GLfloat y2, GLfloat z1, GLfloat z2)
{
   glColor3f (1.0, 1.0, 1.0);

   glBegin (GL_LINE_LOOP);
       glVertex3f (x1, y1, -z1);
       glVertex3f (x2, y1, -z1);
       glVertex3f (x2, y2, -z1);
       glVertex3f (x1, y2, -z1);
   glEnd ();

   glBegin (GL_LINE_LOOP);
       glVertex3f (x1, y1, -z2);
       glVertex3f (x2, y1, -z2);
       glVertex3f (x2, y2, -z2);
       glVertex3f (x1, y2, -z2);
   glEnd ();

   glBegin (GL_LINES);  /*  4 lines     */
       glVertex3f (x1, y1, -z1);
       glVertex3f (x1, y1, -z2);
       glVertex3f (x1, y2, -z1);
       glVertex3f (x1, y2, -z2);
       glVertex3f (x2, y1, -z1);
       glVertex3f (x2, y1, -z2);
       glVertex3f (x2, y2, -z1);
       glVertex3f (x2, y2, -z2);
   glEnd ();
}



/*************************************************************/

// initialize OpenGL  
void InitGL(int Width, int Height)           
{
        
    glEnable(GL_TEXTURE_2D);                  // Enable Texture Mapping
    glClearColor(0.15f, 0.15f, 0.25f, 0.0f);  // Clear The Background Color To Blue 
    glClearDepth(1.0);                        // Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);                     // The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);                  // Enables Depth Testing
    glShadeModel(GL_SMOOTH);                  // Enables Smooth Color Shading
    

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                     
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);   // Calculate The Aspect Ratio Of The Window
    
    //glMatrixMode(GL_MODELVIEW);
}




/***************************************************************/

/*
    function to convert internal geom into GLfloat array 
    this is used to draw points into a Vertex buffer object 
*/

void dump_points_GLfloat( GLfloat* verts, obj_model* pt_object, int numpts)
{
    
    int y = 0;
    int num_gl_pts = 0;
   
    for (y=0;y<numpts;y++)
    {
        
        vec3 point = pt_object->points[y];
        
        verts[num_gl_pts] =  (GLfloat)point.x; num_gl_pts++;
        verts[num_gl_pts] =  (GLfloat)point.y; num_gl_pts++;
        verts[num_gl_pts] =  (GLfloat)point.z; num_gl_pts++;
        verts[num_gl_pts] =  (GLfloat)1.0f; num_gl_pts++; // dont know what the 4th is ??????  

    }

}





/***************************************/
void set_screen_square(int* sx, int* sy){
    
    if (*sx>*sy){
        *sy = *sx;
        glutReshapeWindow(*sx, *sx);
    }else{
        *sx = *sy;
        glutReshapeWindow(*sy, *sy);
    }

}


/***************************************/

void swimbag_tile(void)
{
    int grd_num       = 10;

    float grd_size    = 2.5;
    float gspac = grd_size/(grd_num/2);

    float id = 0;
    glBegin(GL_LINES);
        for(id=0; id<=grd_size; id+=gspac)
        {
            glVertex3f(-id, 0,  grd_size);
            glVertex3f(-id, 0, -grd_size);  
            glVertex3f(-id, 0, -id);
            glVertex3f( id, 0, -id);                
        }
    glEnd();
}



/***************************************/

//draw a 3D grid on the "floor" and an indicator to show XYZ axis  
void graticulate( bool *draw_grid, bool *draw_cntrgrid, RGBType *pt_gridcolor, RGBType *pt_gridcolor2 )
{
 
    int grd_num       = 10;
    
    float grd_height  = 0.0;
    float grd_size    = 2.5;
    float gspac = grd_size/(grd_num/2);

    glBindTexture(GL_TEXTURE_2D, texture[0]);    

    float id = 0;

    glMaterialfv(GL_FRONT, GL_EMISSION, emis_half);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

    glBegin(GL_LINES);
  
        for(id=-gspac; id<=grd_size; id+=gspac)
        {
            if(id==0)
            {  
                if (*draw_cntrgrid == 1)
                {
                    
                    glMaterialfv(GL_FRONT, GL_EMISSION, emis_red);    
                    glVertex3f(  grd_size, grd_height, id );
                    glVertex3f( -grd_size, grd_height, id ); 

                    glMaterialfv(GL_FRONT, GL_EMISSION, emis_green); 
                    glVertex3f( id, grd_size , id );
                    glVertex3f( id, id       , id ); 

                    glMaterialfv(GL_FRONT, GL_EMISSION, emis_blue); 
                    glVertex3f( id, grd_height,   grd_size );
                    glVertex3f( id, grd_height,  -grd_size );  


                }
                if (*draw_grid == 1)
                {
                    glVertex3f( id, grd_height,   grd_size );
                    glVertex3f( id, grd_height,  -grd_size );  

                    glVertex3f(  grd_size, grd_height, id );
                    glVertex3f( -grd_size, grd_height, id ); 
                }   


            }else if (*draw_grid == 1) 
            {
                glMaterialfv(GL_FRONT, GL_EMISSION, emis_half);

                glVertex3f(-id, grd_height,  grd_size);
                glVertex3f(-id, grd_height, -grd_size);  
                glVertex3f( id, grd_height,  grd_size);
                glVertex3f( id, grd_height, -grd_size); 
                glVertex3f(-grd_size, grd_height, -id);
                glVertex3f( grd_size, grd_height, -id);                
                glVertex3f(-grd_size, grd_height,  id);
                glVertex3f( grd_size, grd_height,  id);                 
            }
            

        }

    glEnd();

    if (*draw_cntrgrid==1)
    {
        glColor3i(pt_gridcolor2->r, pt_gridcolor2->g, pt_gridcolor2->b);  
        glBegin(GL_TRIANGLES); 

            glMaterialfv(GL_FRONT, GL_EMISSION, emis_red); 
            glVertex3f(grd_size    , grd_height , -.1 );
            glVertex3f(grd_size    , grd_height ,  .1 );
            glVertex3f(grd_size+.3 , grd_height ,   0 );

            glMaterialfv(GL_FRONT, GL_EMISSION, emis_blue); 
            glVertex3f( -.1, grd_height, grd_size     );
            glVertex3f(  .1, grd_height, grd_size     );
            glVertex3f(   0, grd_height, grd_size+.3  );

        glEnd();
    }
    

    glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);
}


/***************************************/



void show_bbox(bool *pt_draw_bbox, struct obj_info* pt_obinfo, RGBType *pt_gridcolor){

   if (*pt_draw_bbox == 1)
   {
        glMaterialfv(GL_FRONT, GL_EMISSION, emis_half);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_off);

        glBindTexture(GL_TEXTURE_2D, texture[0]);    

        float id = 0;

        glBegin(GL_LINES);
            
            glColor3f(pt_gridcolor->r, pt_gridcolor->g, pt_gridcolor->b);  


            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);

            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z);            
            
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z); 

            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z);
              
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z);            

            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z); 

            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z); 

            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z); 

            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z); 

            glVertex3f(pt_obinfo->bb_min_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z); 

            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_max_y,  pt_obinfo->bb_max_z);

            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_min_z);
            glVertex3f(pt_obinfo->bb_max_x, pt_obinfo->bb_min_y,  pt_obinfo->bb_max_z);

        glEnd();

        glMaterialfv(GL_FRONT, GL_EMISSION, emis_off);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, emis_full);
    }

}








/*************************************************************/

/*

internal inline void
GLSetArrayBuffer(u32 ArrayBuffer, u32 Size, void *Data)
{
    glBindBuffer(GL_ARRAY_BUFFER, ArrayBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, Size, Data);
}

internal u32
GLGenArrayBuffer(u32 SizeInBytes, void *Data, u32 Mode)
{
    u32 ArrayBuffer;
    glGenBuffers(1, &ArrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, ArrayBuffer);
    glBufferData(GL_ARRAY_BUFFER, SizeInBytes, Data, Mode);
    return (ArrayBuffer);
}

internal inline void
GLAttribute(u32 Index, u32 Count, u32 Type = GL_FLOAT, u32 Stride = 0, u32 Offset = 0)
{
    glEnableVertexAttribArray(Index);
    glVertexAttribPointer(Index, Count, Type, GL_FALSE, Stride, (char *)0 + Offset);
}

internal inline void
GLBindVBO(u32 VBO)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

internal inline void
GLBindVAO(u32 VAO)
{
    glBindVertexArray(VAO);
}

internal inline void
GLDrawLines(u32 NumLines)
{
    glDrawArrays(GL_LINES, 0, NumLines * 2);
}

internal inline u32
GLGenVertexArray()
{
    u32 VertexArray;
    glGenVertexArrays(1, &VertexArray);
    glBindVertexArray(VertexArray);
    return (VertexArray);
}

*/

/*

bool check_shader_compile_status(GLuint obj) {
    GLint status;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
        //vector<char> log(length);
        //glGetShaderInfoLog(obj, length, &length, &log[0]);
        //cout << &log[0];
        return false;
    }
    return true;
}



// helper to check and display for shader linker error
bool check_program_link_status(GLuint obj) {
    GLint status;
    glGetProgramiv(obj, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        GLint length;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
        //vector<char> log(length);
        //glGetProgramInfoLog(obj, length, &length, &log[0]);
        //cout << &log[0];
        return false;
    }
    return true;
}

*/



/*
//static GLfloat g_nearPlane = 1;
//static GLfloat g_farPlane = 1000;

    // GLfloat mycolour[3] = {0,0,1}; // blue
    // glColor3fv( mycolour );        // blue using vector of floats
    // glColor3f(1.0, 0.0, 0.0);      // red using floats
    // glColor3ub(0,255,0);           // green using unsigned bytes
    
    //glClearcolor(1.0, 1.0, 1.0, 0.0);   // sets the clear colour to white and opaque
    //glClear( GL_COLOR_BUFFER_BIT);      // clears the colour frame buffer


GLuint load_and_compile_shader(const char *fname, GLenum shaderType) {
    // std::vector<char> buffer;
    // read_shader_src(fname, buffer);
    // const char *src = &buffer[0];
    // // Compile the shader
    // GLuint shader = glCreateShader(shaderType);
    // glShaderSource(shader, 1, &src, NULL);
    // glCompileShader(shader);
    // // Check the result of the compilation
    // GLint test;
    // glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
    // if(!test) {
    //   std::cerr << "Shader compilation failed with this message:" << std::endl;
    //   std::vector<char> compilation_log(512);
    //   glGetShaderInfoLog(shader, compilation_log.size(), NULL, &compilation_log[0]);
    //   std::cerr << &compilation_log[0] << std::endl;
    //   glfwTerminate();
    //   exit(-1);
    // }
    // return shader;
}

void shader_test(void){
  
    char *path_vert_shader = "hello";
    GLuint vertexShader = load_and_compile_shader(path_vert_shader, GL_VERTEX_SHADER);
    
    printf("shader test ran!\n");

    
}
*/




/*
//glewInit();
 
GLuint vbo_triangle, vbo_triangle_colors;
GLint attribute_coord2d, attribute_v_color;

GLfloat triangle_colors[] = {
   1.0, 1.0, 0.0,
   0.0, 0.0, 1.0,
   1.0, 0.0, 0.0,
};
  
glGenBuffers(1, &vbo_triangle_colors);
//glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle_colors);
//glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_colors), triangle_colors, GL_STATIC_DRAW);


//attribute_name = "v_color";
// attribute_v_color = glGetAttribLocation(program, attribute_name);
// if (attribute_v_color == -1) {
//   cerr << "Could not bind attribute " << attribute_name << endl;
//   return false;
// }
*/




// vec3 Intensity = Intensity(ambient) * DiffuseColor + Intensity(diffuse) * DiffuseColor + Intensity(specular) * SpecularColo
// vec3 I = Ia * Kd + Id * Kd + Is * Ks
// vec3 I = (Ia + Id) * Kd + Is * Ks

// vec3 Intensity(diffuse) = Intensity(incoming_lightray) * DiffuseSurfaceColor * cosine(phi)
// vec3 Intensity(specular) = Intensity(incoming_lightray) * SpecularSurfaceColor * cosine(phi) ^ Shininess


/**************************************************************/
/**************************************************************/
// selection demo 

/*


void processHits (GLint hits, GLuint buffer[])
{
   unsigned int i, j;
   GLuint names, *ptr;

   printf ("hits = %d\n", hits);
   ptr = (GLuint *) buffer;
   for (i = 0; i < hits; i++) {  //  for each hit   
      names = *ptr;
      printf (" number of names for hit = %d\n", names); ptr++;
      printf("  z1 is %g;", (float) *ptr/0x7fffffff); ptr++;
      printf(" z2 is %g\n", (float) *ptr/0x7fffffff); ptr++;
      printf ("   the name is ");
      for (j = 0; j < names; j++) {     //  for each name  
         printf ("%d ", *ptr); ptr++;
      }
      printf ("\n");
   }
}


#define BUFSIZE 512

void selectObjects(void)
{
   GLuint selectBuf[BUFSIZE];
   GLint hits;

   glSelectBuffer (BUFSIZE, selectBuf);
   (void) glRenderMode (GL_SELECT);

   glInitNames();
   glPushName(0);

   glPushMatrix ();
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   glOrtho (0.0, 5.0, 0.0, 5.0, 0.0, 10.0);
   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();
   glLoadName(1);
   drawTriangle (2.0, 2.0, 3.0, 2.0, 2.5, 3.0, -5.0);
   glLoadName(2);
   drawTriangle (2.0, 7.0, 3.0, 7.0, 2.5, 8.0, -5.0);
   glLoadName(3);
   drawTriangle (2.0, 2.0, 3.0, 2.0, 2.5, 3.0, 0.0);
   drawTriangle (2.0, 2.0, 3.0, 2.0, 2.5, 3.0, -10.0);
   glPopMatrix ();
   glFlush ();

   hits = glRenderMode (GL_RENDER);
   processHits (hits, selectBuf);
} 

void drawScene (void)
{
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective (40.0, 4.0/3.0, 1.0, 100.0);

   glMatrixMode (GL_MODELVIEW);
   glLoadIdentity ();
   gluLookAt (7.5, 7.5, 12.5, 2.5, 2.5, -5.0, 0.0, 1.0, 0.0);
   glColor3f (0.0, 1.0, 0.0);   //  green triangle       
   drawTriangle (2.0, 2.0, 3.0, 2.0, 2.5, 3.0, -5.0);
   glColor3f (1.0, 0.0, 0.0);   //  red triangle         
   drawTriangle (2.0, 7.0, 3.0, 7.0, 2.5, 8.0, -5.0);
   glColor3f (1.0, 1.0, 0.0);   //  yellow triangles     
   drawTriangle (2.0, 2.0, 3.0, 2.0, 2.5, 3.0, 0.0);
   drawTriangle (2.0, 2.0, 3.0, 2.0, 2.5, 3.0, -10.0);
   drawViewVolume (0.0, 5.0, 0.0, 5.0, 0.0, 10.0);
}

void select_render_loop(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   drawScene ();
   selectObjects ();
   glFlush();
}

*/




