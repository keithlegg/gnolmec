#ifndef SCENELOADER_H    
#define SCENELOADER_H






void save_matrix44(char* filepath, m44 *input );


void read_scenefile(char* scenepath );


//void write_scenefile(char*objpath, m44 *camera, m44 *projection, char* scenefilepath );
void write_scenefile(char*objpath, char*cammatrixpath, char* scenefilepath );





#endif