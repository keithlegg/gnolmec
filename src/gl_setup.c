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

#include "framebuffer.h"
#include "gl_setup.h"


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


/***************************************************************/


// initialize OpenGL  
void InitGL(int Width, int Height)          // We call this right after our OpenGL window is created.
{
        
    glEnable(GL_TEXTURE_2D);             // Enable Texture Mapping
    glClearColor(0.15f, 0.15f, 0.25f, 0.0f); // Clear The Background Color To Blue 
    glClearDepth(1.0);                   // Enables Clearing Of The Depth Buffer
    glDepthFunc(GL_LESS);                // The Type Of Depth Test To Do
    glEnable(GL_DEPTH_TEST);             // Enables Depth Testing
    glShadeModel(GL_SMOOTH);             // Enables Smooth Color Shading
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();                    // Reset The Projection Matrix

    //gluOrtho2D(0, 100, 0, 100); //(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
    gluPerspective(45.0f,(GLfloat)Width/(GLfloat)Height,0.1f,100.0f);   // Calculate The Aspect Ratio Of The Window
    
    glMatrixMode(GL_MODELVIEW);
}










