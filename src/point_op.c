#include <stdlib.h>
#include <stdio.h>
#include <cmath>


#include "math_op.h"
#include "point_op.h"






/*****************************/

/*
 Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
 intersect the intersection point may be stored in the floats i_x and i_y.
 http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
*/

/*
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}
*/

int get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, 
    float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)
{
    float s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
    s10_x = p1_x - p0_x;
    s10_y = p1_y - p0_y;
    s32_x = p3_x - p2_x;
    s32_y = p3_y - p2_y;

    denom = s10_x * s32_y - s32_x * s10_y;
    if (denom == 0)
        return 0; // Collinear
    bool denomPositive = denom > 0;

    s02_x = p0_x - p2_x;
    s02_y = p0_y - p2_y;
    s_numer = s10_x * s02_y - s10_y * s02_x;
    if ((s_numer < 0) == denomPositive)
        return 0; // No collision

    t_numer = s32_x * s02_y - s32_y * s02_x;
    if ((t_numer < 0) == denomPositive)
        return 0; // No collision

    if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
        return 0; // No collision
    // Collision detected
    t = t_numer / denom;
    if (i_x != NULL)
        *i_x = p0_x + (t * s10_x);
    if (i_y != NULL)
        *i_y = p0_y + (t * s10_y);

    return 1;
}

/*****************************/
//not convinced this totally works due to roundoff error, but its close enough
void calc_circle ( pix_coord *out_coords, int numdiv, int x_orig, int y_orig, float dia, int *num)
{
    int divamt = (int)(360/numdiv);
    int rotation_offset = 45;

    for (int i = 0; i <360; i=i+divamt)
    {  
        out_coords[*num].x = x_orig + (sin(dtr(i-rotation_offset))*dia);
        out_coords[*num].y = y_orig + (cos(dtr(i-rotation_offset))*dia);
        *num = *num+1;
    }

}

/*****************************/

void calc_line( pix_coord *out_coords, int *pt1, int *pt2, int *num)
{

    int x1 = pt1[0];
    int y1 = pt1[1];
    int const x2 = pt2[0];
    int const y2 = pt2[1];

    int const foobar = 100;

    short xy_idx     = 0;
    int delta_x(x2 - x1);

    // if x1 == x2, then it does not matter what we set here
    signed char const ix((delta_x > 0) - (delta_x < 0));
    delta_x = abs(delta_x) << 1;
    int delta_y(y2 - y1);
    // if y1 == y2, then it does not matter what we set here
    signed char const iy((delta_y > 0) - (delta_y < 0));
    delta_y = abs(delta_y) << 1;
    
    //dump a point
    
    out_coords[xy_idx].x = x1;
    out_coords[xy_idx].y = y1;
    xy_idx++;


    if (delta_x >= delta_y)
    {
      // error may go below zero
      int error(delta_y - (delta_x >> 1));
      while (x1 != x2)
      {
          if ((error >= 0) && (error || (ix > 0)))
          {
              error -= delta_x;
              y1 += iy;
          }
          // else do nothing
          error += delta_y;
          x1 += ix;
            //dump  a point
            out_coords[xy_idx].x = x1;
            out_coords[xy_idx].y = y1;
            xy_idx++;

      }
    }
    else
    {
      // error may go below zero
      int error(delta_x - (delta_y >> 1));
      while (y1 != y2)
      {
          if ((error >= 0) && (error || (iy > 0)))
          {
              error -= delta_y;
              x1 += ix;
          }
          // else do nothing
          error += delta_x;
          y1 += iy;

            //dump  a point
            out_coords[xy_idx].x = x1;
            out_coords[xy_idx].y = y1;
            xy_idx++;
      }
    }

   *num = xy_idx;
}

/*****************************/


