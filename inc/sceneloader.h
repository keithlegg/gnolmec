#ifndef SCENELOADER_H    
#define SCENELOADER_H



#include "math_op.h"



void clear_scenegeom( void );


void load_matrix44(char* filename, m44 * pm44 );
void load_matrix33(char* filename, m33 * pm33 );

void save_matrix44(char* filepath, m44 *input );
void save_matrix33(char* filepath, m33 *input );


void read_scenefile(char* scenepath );


//void write_scenefile(char*objpath, m44 *camera, m44 *projection, char* scenefilepath );
void write_scenefile(char*objpath, char*cammatrixpath, char* scenefilepath );





#endif