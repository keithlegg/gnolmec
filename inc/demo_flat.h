#ifndef FLATIMAGEDEMO_H    
#define FLATIMAGEDEMO_H


#include "framebuffer.h" // raster operations

void drawglscene_2d();
int BuildPopupMenu (void);
void MouseButton(int button, int state, int x, int y);
void MouseMotion(int x, int y);

void flatImageDemo(int *argc, char** argv);



#endif