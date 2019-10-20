#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cmath>

#include "framebuffer.h"   
#include "point_op.h"  

#include <string.h> //for memcopy

/*********************************/

/*

    Evolved from graphics (obviously), scanning fiducials (info in bitmaps), eggcarton analogy 

    Weird idea to use a bitmap to mock up a "CPU"
        - store "bits" as blocks of images 
        - you could then save the computation and animate it to see it "running"

    goal is to build:
       microcode 
       assembler
       compiler 
  
*/


/*********************************/

/*

    gridify() - allocates memory 
        text file for ram setup   6502/SAP/SCOTT CPU/ETC 
        changes a block in "ram"

    read_memory
    write_memory 
    

    program counter 
    stack pointer 


*/


/*********************************/


int CLOCK_TICK = 0;
int PC = 0;
int SP = 0;
int ACCUM = 0;

int AREG = 0;
int BREG = 0;
