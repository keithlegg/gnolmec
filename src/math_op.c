
#include <stdlib.h>
#include <stdio.h>

#include <cmath>

#include "math_op.h"


/*********************************************************/
/*

   This is an attempt to port the python math library I wrote into 
   the C language

   The original can be found here:

       https://github.com/keithlegg/pyrender2/blob/master/pygfx/math_ops.py



*/

/*********************************************************/






/*********************************************************/
/*General linear algebra and trig functions 
  these can be used by higher order objects like vectors and matricies, 
  or used directly */
/*********************************************************/


//degree to radian 
double deg_to_rad ( double deg){
   return deg * DEG_TO_RAD;
}

//radian to degree
double rad_to_deg ( double rad){
   return rad * RAD_TO_DEG;
}

/*****************************/
double dotProduct ( vec2 v1, vec2 v2){
    return (v1.x*v2.x + v1.y*v2.y ); 
}


double dotProduct ( vec3 v1, vec3 v2){
    return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z); 
}


/*****************************/
// distance between 2 double points in 2D
double fcalc_distance(double pt1[2], double pt2[2]){
    return sqrt( ( (pt1[0]-pt2[0])*(pt1[0]-pt2[0])) + ((pt1[1]-pt2[1])*(pt1[1]-pt2[1])) ) ;
}

// distance between 2 int points in 2D
double fcalc_distance(int pt1[2], int pt2[2]){
    return sqrt( ( (pt1[0]-pt2[0])*(pt1[0]-pt2[0])) + ((pt1[1]-pt2[1])*(pt1[1]-pt2[1])) ) ;
}

// distance between 2 vectors in 2D
double fcalc_distance( vec2 input){
    return sqrt( (input.x * input.x) + (input.y * input.y) );
}

// distance between 2 vectors in 3D
double fcalc_distance( vec3 input){
    return sqrt( (input.x * input.x) + (input.y * input.y) + (input.z * input.z) );
}


/*****************************/

double calc_theta_vert ( double start_x, double start_y, double end_x, double end_y)
{
    //#get corner to build a right triangle
    double a_x = end_x-start_x;  
    double a_y = end_y-start_y;

    double r = 0;
    
    //#relative offset (depending on order of start-end)
    if (a_x != 0 && a_y !=0){
         r = ( rad_to_deg( atan(a_x/a_y) ) );
    }
  
    //printf(" angle is %f\n" , r);
    return r;
}

/*****************************/

double angle_between( vec2 v_one, vec2 v_two )
{
    vec2 v1 ;
    vec2 v2 ;
    v1 = normalize(v_one);
    v2 = normalize(v_two);
    double dot = dotProduct(v1,v2);
    return rad_to_deg( acos(dot) );
}


/*****************************/
/*****************************/
/*****************************/

vec2 newvec2( int x, int y ){
    vec2 output;
    output.x = (double)x;
    output.y = (double)y;
    return output; 
}

vec2 newvec2( double x, double y ){
    vec2 output;
    output.x = x;
    output.y = y;
    return output; 
}

/*****************************/

/* normalize a vector */
vec2 normalize( vec2 input )
{
     double length  = fcalc_distance(input);

     vec2 output;

     if(length != 0)
     {
        output.x = input.x/length;
        output.y = input.y/length;
     }

     return output; 
}

/*****************************/
vec2 scale_vec( vec2 input, double amount ){
    vec2 output;
    output.x = input.x/amount;
    output.y = input.y/amount;
    return output; 
}

/*****************************/
vec2 mult_vec_scalar( vec2 input, double amount ){
    vec2 output;
    output.x = input.x*amount;
    output.y = input.y*amount;
    return output; 
}

/*****************************/

/*
  convert an arbitrary line segment to a true vector from origin.
*/
vec2 line2vect(double start_x, double start_y, double end_x, double end_y)
{
    vec2 out;
    out.x = end_x-start_x;
    out.y = end_y-start_y;
    return out;
}

vec2 line2vect(int start_x, int start_y, int end_x, int end_y)
{
    vec2 out;
    out.x = (double)(end_x-start_x);
    out.y = (double)(end_y-start_y);
    return out;
}


/*****************************/

/* multiply two 2d vectors */
vec2 vmul_2d ( vec2 v1, vec2 v2 )
{
     vec2 output;
     
     output.x = v1.x * v2.x;
     output.y = v1.y * v2.y;

     return output; 
}


/************************************************/

vec3 add_vec3 (vec3 v1, vec3 v2){
    vec3 output;
    output.x = v1.x + v2.x;
    output.y = v1.y + v2.y;
    output.z = v1.z + v2.z;
    
    return output;

}


vec3 sub_vec3 (vec3 v1, vec3 v2){
    vec3 output;
    output.x = v1.x - v2.x;
    output.y = v1.y - v2.y;
    output.z = v1.z - v2.z;
    
    return output;    
}

vec3 mult_vec3 (vec3 v1, vec3 v2){
    vec3 output;
    output.x = v1.x * v2.x;
    output.y = v1.y * v2.y;
    output.z = v1.z * v2.z;
    
    return output;
}


vec3 div_vec3 (vec3 v1, vec3 v2){
    // UNTESTED - normalized? 
    vec3 output;

    // self.x = other.x/other.length()
    // self.y = other.y/other.length()
    // self.z = other.y/other.length()
    // return type(self)(self.x, self.y, self.z)
    
    // vec3 output;
    // output.x = v1.x - v2.x;
    // output.y = v1.y - v2.y;
    // output.z = v1.z - v2.z;
    return output;

}


/*****************************/
/*****************************/
/*****************************/

vec3 crossProduct(vec3 v1, vec3 v2){
    // UNTESTED! 
    vec3 output;
    
    output.x =  v1.y*v2.z-v1.z*v2.y; 
    output.y =  v1.z*v2.x-v1.x*v2.z; 
    output.z =  v1.x*v2.y-v1.y*v2.x;
    
    return output;
}

vec3 newvec3( int x, int y, int z ){
    vec3 output;
    output.x = (double)x;
    output.y = (double)y;
    output.z = (double)z;
    return output; 
}

vec3 newvec3( double x, double y, double z ){
    vec3 output;
    output.x = x;
    output.y = y;
    output.z = z;
    return output; 
}

/* normalize a 3d vector */
vec3 normalize( vec3 input )
{
     double length  = fcalc_distance(input);

     vec3 output;

     if(length != 0)
     {
        output.x = input.x/length;
        output.y = input.y/length;
        output.z = input.z/length;

     }

     return output; 
}


vec3 dtr_vec3(vec3 invec){
    vec3 output;

    //return ( deg_to_rad( invec[0] ),
    //         deg_to_rad( invec[1] ),
    //         deg_to_rad( invec[2] ),
    //       )
    
    return output;

}

/*****************************/

vec3 rtd_vec3(vec3 invec){
    vec3 output;    
    // return ( rad_to_deg( invec[0] ),
    //          rad_to_deg( invec[1] ),
    //          rad_to_deg( invec[2] ),
    //        )
    return output;
}        

/*****************************/
vec3 vect_add( vec3 v1, vec3 v2){
    vec3 output;    
    //return [v1[0]+v2[0], v1[0]+v2[1], v1[2]+v2[2]]
    return output;

}

/*****************************/
vec3 mult_scalar (double scalar, vec3 v){
    vec3 output; 
    // return [v[0]*scalar, v[1]*scalar, v[2]*scalar ]
    return output;
}

/*****************************/
/* multiplies a 3X3 matrix by a vec3 
   returns a vec3 */

vec3 mult_m33_vec3(m33 m, vec3 v){
    vec3 output;
    
    output.x = m.m0 * v.x + m.m3 * v.y + m.m6 * v.z; 
    output.y = m.m1 * v.x + m.m4 * v.y + m.m7 * v.z; 
    output.z = m.m2 * v.x + m.m5 * v.y + m.m8 * v.z; 

    return output;
}


/*****************************/
/* multiplies a 4X4 matrix by a 3D vector 
   returns a vector tuple */

vec3 mult_m44_vec3(m44 m, vec3 v){
    vec3 output;

    output.x = m.m0*v.x + m.m4*v.y + m.m8  * v.z+m.m12;
    output.y = m.m1*v.x + m.m5*v.y + m.m9  * v.z+m.m13;
    output.z = m.m2*v.x + m.m6*v.y + m.m10 * v.z+m.m14;
       
    return output;
}




// given 2 points in 3D, create a 3D vector 
// representing the offset between them 
// doesnt get much easier than this, just subtract 
////////////
// vec3 between(double pt1_x, double pt1_y, double pt1_z, 
//                  double pt2_x, double pt2_y, double pt2_z){
//   
//         #if isinstance(pt1, tuple):
//         #    pt1 = vec3( pt1[0], pt1[1], pt1[2]) 
// 
//         if isinstance(pt2, tuple):
//             pt2 = vec3( pt2[0], pt2[1], pt2[2])
// 
//         return pt2 - self
// }


/*

    def project_pt(self, offset):
        """ project a point along a vector """
        
        # nX = B.x - A.x
        # nY = B.y - A.y
        # nZ = B.z - A.z 

        # distX = pow( (A.x - B.x ) , 2.0 ) 
        # distY = pow( (A.y - B.y ) , 2.0 ) 
        # distZ = pow( (A.z - B.z ) , 2.0 ) 
        # vecLength = math.sqrt(distX + distY + distZ )

        # normalized vector  
        # calcX = nX / self.length
        # calcY = nY / self.length
        # calcZ = nZ / self.length

        normal = self.normal

        # project point along vector with offset (can use negative too)
        ptX = self.x + (normal.x * offset)
        ptY = self.y + (normal.y * offset)
        ptZ = self.z + (normal.z * offset)
        return type(self)(ptX, ptY, ptZ)


    def angle_between(self, other):
        """ 
            result is in radians
            derived from law of cosines 
            range from 0 (colinear) to PI radians (180 deg)  

            FAILS WHEN 0 or 180 ?
        """
        try:
            o = math.acos( self.dot(other) / (self.length*other.length) ) 
            return o 
        except:
            return 0

    def vector_mean(self, p_vecs, mode='vec3'):
        """ 
            UNTESTED 
         
            take a list of vectors and return a new vector that is an average

        """
        tmp_x = 0;tmp_y = 0;tmp_z = 0;

        count = 0
        a = 0 
        for v in p_vecs: 

            if mode=='angle':            
                a = a + self.np_angle_between( upvec, v )

            if mode=='vec3':
                if isinstance(v, vec3):
                    tmp_x = tmp_x+v.x
                    tmp_y = tmp_x+v.y
                    tmp_z = tmp_x+v.z 

                if isinstance(v, tuple) or isinstance(v, list):                
                    tmp_x = tmp_x+v[0]
                    tmp_y = tmp_x+v[1]
                    tmp_z = tmp_x+v[2] 

            count += 1 
        
        if mode=='angle':
            return a/count 
        if mode=='vec3':
            return type(self)(tmp_x/count, tmp_y/count, tmp_z/count)


        return None 

*/      


 
void print_vec2(vec2 input){
    printf("%f %f \n", input.x, input.y );
}

void print_vec3(vec3 input){
    printf("%f %f %f\n", input.x, input.y, input.z );
}
 

/*****************************/
/*****************************/
/*****************************/

m33 new_m33( double m0 , double m1 , double m2,  
             double m3 , double m4 , double m5, 
             double m6 , double m7 , double m8
    ){
    m33 output;
    return output; 
}


m44 new_m44( double m0 , double m1 , double m2 , double m3, 
             double m4 , double m5 , double m6 , double m7,
             double m8 , double m9 , double m10, double m11,
             double m12, double m13, double m14, double m15
    ){
    m44 output;
    return output; 
}


m33 identity33(void){

    m33 output;    
     
    output.m0 =  1.0;
    output.m1 =  0;
    output.m2 =  0;
    
    output.m3 =  0;
    output.m4 =  1.0;
    output.m5 =  0;

    output.m6 =  0;
    output.m7 =  0;
    output.m8 =  1;   
                
    return output;  

}


m44 identity44(void){
    m44 output;
    
    output.m0  = 1.0;
    output.m1  = 0;
    output.m2  = 0;
    output.m3  = 0;

    output.m4  = 0;
    output.m5  = 1.0;
    output.m6  = 0;
    output.m7  = 0;

    output.m8  = 0;
    output.m9  = 0;
    output.m10 = 1.0;
    output.m11 = 0;

    output.m12 = 0;
    output.m13 = 0;
    output.m14 = 0;
    output.m15 = 1.0;
    
    return output;
}

/*****************************/

m33 matrix_add(m33 other){
    // self.m[0]+n[0], self.m[1]+n[1], self.m[2]+n[2],
    // self.m[3]+n[3], self.m[4]+n[4], self.m[5]+n[5],
    // self.m[6]+n[6], self.m[7]+n[7], self.m[8]+n[8]     
}

m44 matrix_add(m44 other){
    //  self.m[0] +n[0] , self.m[1]+n[1]  , self.m[2]+n[2]  , self.m[3]+n[3]  , 
    //  self.m[4] +n[4] , self.m[5]+n[5]  , self.m[6]+n[6]  , self.m[7]+n[7]  ,
    //  self.m[8] +n[8] , self.m[9]+n[9]  , self.m[10]+n[10], self.m[11]+n[11],
    //  self.m[12]+n[12], self.m[13]+n[13], self.m[14]+n[14], self.m[15]+n[15]

}

m33 matrix_sub(m33 other){
    //   self.m[0]-n[0], self.m[1]-n[1], self.m[2]-n[2],
    //   self.m[3]-n[3], self.m[4]-n[4], self.m[5]-n[5],
    //   self.m[6]-n[6], self.m[7]-n[7], self.m[8]-n[8]     
}

m44 matrix_sub(m44 other){
    //  self.m[0] -n[0] , self.m[1]-n[1]  , self.m[2]-n[2]  , self.m[3]-n[3]  , 
    //  self.m[4] -n[4] , self.m[5]-n[5]  , self.m[6]-n[6]  , self.m[7]-n[7]  ,
    //  self.m[8] -n[8] , self.m[9]-n[9]  , self.m[10]-n[10], self.m[11]-n[11],
    //  self.m[12]-n[12], self.m[13]-n[13], self.m[14]-n[14], self.m[15]-n[15]

}


/*****************************/
/* multiply two 3X3 matricies together */

m33 mult_mat33(m33 m, m33 n){
    // UNTESTED
    m33 output;    
     
    output.m0 =  m.m0*n.m0 + m.m1*n.m3 + m.m2*n.m6;
    output.m1 =  m.m0*n.m1 + m.m1*n.m4 + m.m2*n.m7;
    output.m2 =  m.m0*n.m2 + m.m1*n.m5 + m.m2*n.m8;
    output.m3 =  m.m3*n.m0 + m.m4*n.m3 + m.m5*n.m6;
    output.m4 =  m.m3*n.m1 + m.m4*n.m4 + m.m5*n.m7;
    output.m5 =  m.m3*n.m2 + m.m4*n.m5 + m.m5*n.m8;
    output.m6 =  m.m6*n.m0 + m.m7*n.m3 + m.m8*n.m6;
    output.m7 =  m.m6*n.m1 + m.m7*n.m4 + m.m8*n.m7;
    output.m8 =  m.m6*n.m2 + m.m7*n.m5 + m.m8*n.m8;   
                
    return output;        
}


/*****************************/
/* multiply two 4X4 matricies together */

m44 mult_mat44(m44 m, m44 n){
    // UNTESTED - NOT DONE 
    m44 output;
    
    output.m0  = m.m0*n.m0  + m.m1*n.m4  + m.m2*n.m8   + m.m3*n.m12;
    output.m1  = m.m0*n.m1  + m.m1*n.m5  + m.m2*n.m9   + m.m3*n.m13;
    output.m2  = m.m0*n.m2  + m.m1*n.m6  + m.m2*n.m10  + m.m3*n.m14;
    output.m3  = m.m0*n.m3  + m.m1*n.m7  + m.m2*n.m11  + m.m3*n.m15;
    output.m4  = m.m4*n.m0  + m.m5*n.m4  + m.m6*n.m8   + m.m7*n.m12;
    output.m5  = m.m4*n.m1  + m.m5*n.m5  + m.m6*n.m9   + m.m7*n.m13;
    output.m6  = m.m4*n.m2  + m.m5*n.m6  + m.m6*n.m10  + m.m7*n.m14;
    output.m7  = m.m4*n.m3  + m.m5*n.m7  + m.m6*n.m11  + m.m7*n.m15;
    output.m8  = m.m8*n.m0  + m.m9*n.m4  + m.m10*n.m8  + m.m11*n.m12;
    output.m9  = m.m8*n.m1  + m.m9*n.m5  + m.m10*n.m9  + m.m11*n.m13;
    output.m10 = m.m8*n.m2  + m.m9*n.m6  + m.m10*n.m10 + m.m11*n.m14;
    output.m11 = m.m8*n.m3  + m.m9*n.m7  + m.m1*n.m11  + m.m11*n.m15;
    output.m12 = m.m12*n.m0 + m.m13*n.m4 + m.m14*n.m8  + m.m15*n.m12;
    output.m13 = m.m12*n.m1 + m.m1*n.m5  + m.m14*n.m9  + m.m15*n.m13;
    output.m14 = m.m12*n.m2 + m.m13*n.m6 + m.m14*n.m10 + m.m15*n.m14;
    output.m15 = m.m12*n.m3 + m.m13*n.m7 + m.m14*n.m11 + m.m15*n.m15;
    
    return output;
}

/*****************************/




m33 copy_matrix( m33 input ){

    m33 output;    
     
    output.m0 =  input.m0;
    output.m1 =  input.m1;
    output.m2 =  input.m2;
    
    output.m3 =  input.m3;
    output.m4 =  input.m4;
    output.m5 =  input.m5;

    output.m6 =  input.m6;
    output.m7 =  input.m7;
    output.m8 =  input.m8;   
                
    return output;  

}

m44 copy_matrix(m44 input){
    m44 output;
    
    output.m0  = input.m0;
    output.m1  = input.m1;
    output.m2  = input.m2;
    output.m3  = input.m3;

    output.m4  = input.m4;
    output.m5  = input.m5;
    output.m6  = input.m6;
    output.m7  = input.m7;

    output.m8  = input.m8;
    output.m9  = input.m9;
    output.m10 = input.m10;
    output.m11 = input.m11;

    output.m12 = input.m12;
    output.m13 = input.m13;
    output.m14 = input.m14;
    output.m15 = input.m15;
    
    return output;
}


m33 test_indices33(void){
     // create a useless matrix with incrementing numbers  
     // for testing transpose and more 

    m33 output;    
     
    output.m0 =  0;
    output.m1 =  1;
    output.m2 =  2;
    
    output.m3 =  3;
    output.m4 =  4;
    output.m5 =  5;

    output.m6 =  6;
    output.m7 =  7;
    output.m8 =  8;   
                
    return output;      

}

m33 determinant(void){
    //     https://www.mathsisfun.com/algebra/matrix-determinant.html
    //           a b c  |  0 1 2  |        
    //      A =  d e f  |  3 4 5  |       
    //           g h i  |  6 7 8  |     
    //     |A| = a(ei − fh) − b(di − fg) + c(dh − eg)
    //     |A| = 0(48 − 57) − 1(38 − 56) + 2(37 − 46)
    
    m33 output;    
     
    output.m0 =  1.0;
    output.m1 =  0;
    output.m2 =  0;
    
    //     a = self.copy() 
    //     o = a[0]* ((a[4]*a[8])-(a[5]*a[7])) - a[1]*((a[3]*a[8])-(a[5]*a[6])) +  a[2]*((a[3]*a[7])-(a[4]*a[6]))
    //     return o

                
    return output;  

}


/* standard indicies  |   transposed indicies
   1  2  3            |   1 4 7
   4  5  6            |   2 5 8
   7  8  9            |   3 6 9  */

m33 transpose(m33 input){
    m33 output;  
    
    output.m0 = input.m0;
    output.m1 = input.m3;
    output.m2 = input.m6;

    output.m3 = input.m1;
    output.m4 = input.m4;
    output.m5 = input.m7;

    output.m6 = input.m2;
    output.m7 = input.m5;
    output.m8 = input.m8;    

    return output;     
}


void print_matrix(m33 input){
    printf("%f %f %f\n", input.m0, input.m1, input.m2 );
    printf("%f %f %f\n", input.m3, input.m4, input.m5 );
    printf("%f %f %f\n", input.m6, input.m7, input.m8 );    
}

void print_matrix(m44 input){
    printf("%f %f %f %f\n", input.m0  , input.m1  , input.m2  , input.m3 );
    printf("%f %f %f %f\n", input.m4  , input.m5  , input.m6  , input.m7 );
    printf("%f %f %f %f\n", input.m8  , input.m9  , input.m10 , input.m11 );
    printf("%f %f %f %f\n", input.m12 , input.m13 , input.m14 , input.m15 );        
}




/* multiply two 3X3 matricies together 
   the order that you mutliply will call a different object!!!
   make sure you do "this * other", NOT "other * this"
*/



    // m33 from_euler(self, xrot, yrot, zrot):
    //     dtr = self.mu.dtr
    //     # build rotationY (see diagram above) 
    //     y_matrix =  self.identity
    //     y_matrix[0]  =  math.cos(dtr( yrot ))
    //     y_matrix[2]  = -math.sin(dtr( yrot ))
    //     y_matrix[6]  =  math.sin(dtr( yrot ))
    //     y_matrix[8]  =  math.cos(dtr( yrot ))
    //     ####                
    //     # build rotationZ (see diagram above) 
    //     z_matrix    =  self.identity
    //     z_matrix[0] =  math.cos(dtr( zrot ))
    //     z_matrix[1] =  math.sin(dtr( zrot ))
    //     z_matrix[3] = -math.sin(dtr( zrot ))
    //     z_matrix[4] =  math.cos(dtr( zrot ))
    //     tmp_matr =  y_matrix * z_matrix 
    //     ####
    //     # build rotationX (see diagram above) 
    //     x_matrix =  self.identity
    //     x_matrix[4]  =   math.cos(dtr( xrot )) 
    //     x_matrix[5]  =   math.sin(dtr( xrot )) 
    //     x_matrix[7]  =  -math.sin(dtr( xrot ))
    //     x_matrix[8]  =   math.cos(dtr( xrot ))
    //     rotation_33 = x_matrix * tmp_matr 
    //     self.insert(rotation_33)

    // m33 rotate_pts_3d(points, xrot, yrot, zrot):
    //     //  The "standard" 9 element, Row major, 3X3 rotation matrix used by Maya
    //     //   ⎡0  1  2⎤      xx xy xz 
    //     //   ⎢3  4  5⎥      yx yy yz 
    //     //   ⎣6  7  8⎦      zx zy zz 
    //     //   ------------------------------
    //     //        Rotate Y matrix     
    //     //   ⎡cos(y)  0      -sin(y) ⎤ 
    //     //   ⎢0       1       0      ⎥ 
    //     //   ⎣sin(y)  0       cos(y) ⎦ 
    //     //   ------------------------------
    //     //        Rotate Z  matrix 
    //     //   ⎡  cos(z)  sin(z)  0    ⎤ 
    //     //   ⎢ -sin(z)  cos(z)  0    ⎥
    //     //   ⎣  0       0       1    ⎦
    //     //   ------------------------------
    //     //        Rotate X matrix  
    //     //   ⎡ 1       0         0   ⎤    
    //     //   ⎢ 0    cos(x)   sin(x)  ⎥  
    //     //   ⎣ 0   -sin(x)   cos(x)  ⎦  
    //     //   ------------------------------            
    //     dtr = self.mu.dtr
    //     # build rotationY (see diagram above) 
    //     y_matrix =  self.identity
    //     y_matrix[0]  =  math.cos(dtr( yrot ))
    //     y_matrix[2]  = -math.sin(dtr( yrot ))
    //     y_matrix[6]  =  math.sin(dtr( yrot ))
    //     y_matrix[8]  =  math.cos(dtr( yrot ))
    //     ####                
    //     # build rotationZ (see diagram above) 
    //     z_matrix    =  self.identity
    //     z_matrix[0] =  math.cos(dtr( zrot ))
    //     z_matrix[1] =  math.sin(dtr( zrot ))
    //     z_matrix[3] = -math.sin(dtr( zrot ))
    //     z_matrix[4] =  math.cos(dtr( zrot ))
    //     tmp_matr =  y_matrix * z_matrix 
    //     ####
    //     # build rotationX (see diagram above) 
    //     x_matrix =  self.identity
    //     x_matrix[4]  =   math.cos(dtr( xrot )) 
    //     x_matrix[5]  =   math.sin(dtr( xrot )) 
    //     x_matrix[7]  =  -math.sin(dtr( xrot ))
    //     x_matrix[8]  =   math.cos(dtr( xrot ))
    //     rotation_33 = x_matrix * tmp_matr 
    //     ############ 
    //     return rotation_33.batch_mult_pts(points)



    // m33 batch_mult_pts(self, pts):
    //     // iterate a list of points and multiply them by this matrix 
    //     tmp_buffer = []
    //     out = None
    //     for pvec in pts:  
    //         tmp_buffer.append( self * pvec )
    //     return tmp_buffer


    // m33 align_two_vec3(self, a, b):
    //     //UNFINISHED
    //     //     https://math.stackexchange.com/questions/180418/calculate-rotation-matrix-to-align-vector-a-to-vector-b-in-3d
    //     //      ⎛cosθ −sinθ  0 ⎞
    //     //  G = ⎜sinθ  cosθ  0 ⎟ 
    //     //      ⎝0      0    1 ⎠
    //     //       --------------------------------------------------------------
    //     //       Given our unit vectors, we note that cosθ=A⋅B, and sinθ=||A×B||
    //     //      ⎛ A.B    -||A×B||    0 ⎞
    //     //  G = ⎜||A×B||   A.B       0 ⎟ 
    //     //      ⎝0          0        1 ⎠
    //     theta = self.mu.dtr(angle)
    //     #axis = vec3.normal
    //     axis = vec3.as_np
    //     #tmpm33 = self.identity
    //     tmpm33 = expm(np.cross(np.eye(3), axis / np.linalg.norm(axis) * theta)) 
    //     return self.from_np(tmpm33)




/*************************************************************************/

quaternion new_quaternion( double x, double y, double z, double w ){
    quaternion output;

    output.x = x;
    output.y = y;
    output.z = z;
    output.w = w;

    return output; 
}

quaternion quaternion_identity(){
    return new_quaternion(1.0,0,0,0);
}


void print_quaternion(quaternion input){
    printf("%f %f %f %f\n", input.x, input.y, input.z, input.w );
}


void quaternion_rotx(quaternion *input, double theta){
    double theta_over2 = theta * .5;
    //input.w = math.cos(theta_over2);
    //input.x = math.sin(theta_over2);
}


void quaternion_roty(quaternion *input, double theta){
    double theta_over2 = theta * .5;
    // input.w = math.cos(theta_over2)
    // input.y = math.sin(theta_over2)
}


void quaternion_rotz(quaternion *input, double theta){
    double theta_over2 = theta * .5;
    // input.w = math.cos(theta_over2)
    // input.z = math.sin(theta_over2)
}


void quaternion_fr_euler(quaternion *input, double h, double p, double b){
    // sp=0;sb=0;sh=0
    // cp=0;cb=0;ch=0
    // sp = math.sin(p*.5) 
    // sb = math.sin(b*.5) 
    // sh = math.sin(h*.5) 
    // cp = math.cos(p*.5) 
    // cb = math.cos(b*.5) 
    // ch = math.cos(h*.5) 
       
    // if (trans_type == 'obj2inertial'):
    //     self.w = ch * cp * cb + sh * sp * sb
    //     self.x = ch * sp * cb + sh * cp * sb
    //     self.y = -ch * sp * sb + sh * cp * cb
    //     self.z = -sh * sp * cb + ch * cp * sb
    // elif (trans_type == 'inertial2ob'):
    //     self.w = ch * cp * cb + sh * sp * sb
    //     self.x = -ch * sp * cb - sh * cp * sb
    //     self.y = ch * sp * sb - sh * cp * cb
    //     self.z = sh * sp * cb - ch * cp * sb
    // else:
    //     printf( "Invalid trans_type!" ) 
}


void quaternion_mag(quaternion *input){
    //mag = float( math.sqrt(  self.w*self.w + 
    //                         self.x*self.x + 
    //                         self.y*self.y + 
    //                         self.z*self.z) )     
}


void quaternion_normalize(quaternion *input){
    // mag = self.mag() 
    // if mag > 0:
    //     oneOverMag = float( 1.0 / mag)
    //     self.w *= oneOverMag
    //     self.x *= oneOverMag
    //     self.y *= oneOverMag
    //     self.z *= oneOverMag
    // else:
    //     self.set_identity()   
}

/*
    def quaternion_dot_product(self, q): 
        return a.x * b.x + a.y * b.y + a.z * b.z + a.z * a.z;   

    def quaternion_conjugate(self, q):
        result = type(self)()
        
        result.w =  q.w
        result.x = -q.x
        result.y = -q.y
        result.z = -q.z
        return result

    def quaternion_mul__(self, a):
        result = type(self)()
    
        w = self.w
        x = self.x
        y = self.y
        z = self.z

        result.w = w * a.w - x * a.x - y * a.y - z * a.z
        result.x = w * a.x + x * a.w + z * a.y + y * a.z
        result.y = w * a.y + y * a.w + x * a.z + z * a.x
        result.z = w * a.z + z * a.w + y * a.x + x * a.y
        
        return result;


    def quaternion_from_m33(self, m33):
        """ 
           m11  m12 m13 
           m21  m22 m23 
           m31  m32 m33 
        """
        
        four_w_sq_min1 = m33[0] + m33[4] + m33[8]
        four_x_sq_min1 = m33[0] - m33[4] - m33[8]
        four_y_sq_min1 = m33[4] - m33[0] - m33[8]
        four_z_sq_min1 = m33[8] - m33[0] - m33[4]
        
        maxIndex = 0
        max = four_w_sq_min1
        
        if four_x_sq_min1 > max:
            max = four_x_sq_min1
            maxIndex = 1
        
        if (four_y_sq_min1 > max):
            max = four_y_sq_min1
            maxIndex = 2
        
        if (four_z_sq_min1 > max):
            max = four_z_sq_min1
            maxIndex = 3
        
        max = math.sqrt (max + 1.0) * 0.5
        mult = 0.25 / max
        
        if maxIndex==0:
            self.w = max;
            self.x = (m33[5] - m33[7]) * mult;
            self.y = (m33[6] - m33[2]) * mult;
            self.z = (m33[1] - m33[3]) * mult;

        if maxIndex==1:
            self.x = max;
            self.w = (m33[5] - m33[7]) * mult;
            self.y = (m33[1] + m33[3]) * mult;
            self.z = (m33[6] + m33[2]) * mult;

        if maxIndex==2:
            self.y = max;
            self.w = (m33[6] - m33[2]) * mult;
            self.x = (m33[1] + m33[3]) * mult;
            self.z = (m33[5] + m33[7]) * mult;

        if maxIndex==3:
            self.z = max;
            self.w = (m33[1] - m33[3]) * mult;
            self.x = (m33[6] + m33[2]) * mult;
            self.y = (m33[5] + m33[7]) * mult;
            self.z = (m33[5] + m33[7]) * mult;
 


    def quaternion_to_m33(self, trans_type='inertial2obj'):
 
            mo = matrix33() 
            q = self 

            if (trans_type == 'inertial2obj'):
 
                mo[0] = 1.0 - 2.0 * (q.y * q.y + q.z * q.z)
                mo[1] = 2.0 * (q.x * q.y + q.w * q.z)
                mo[2] = 2.0 * (q.x * q.z + q.w * q.y)
                
                mo[3] = 2.0 * (q.x * q.y - q.w * q.z)
                mo[4] = 1.0 - 2.0 * (q.x * q.x + q.z * q.z)
                mo[5] = 2.0 * (q.y * q.z + q.w * q.x)
                
                mo[6] = 2.0 * (q.x * q.z + q.w * q.y)
                mo[7] = 2.0 * (q.y * q.z - q.w * q.x)
                mo[8] = 1.0 - 2.0 * (q.x * q.x + q.y * q.y)
 
                return mo 

            elif (trans_type == 'obj2inertial'):
 
                mo[0] = 1.0 - 2.0 * (q.y * q.y + q.z * q.z)
                mo[1] = 2.0 * (q.x * q.y - q.w * q.z)
                mo[2] = 2.0 * (q.x * q.z + q.w * q.y)
                
                mo[3] = 2.0 * (q.x * q.y + q.w * q.z)
                mo[4] = 1.0 - 2.0 * (q.x * q.x + q.z * q.z)
                mo[5] = 2.0 * (q.y * q.z - q.w * q.x)

                mo[6] = 2.0 * (q.x * q.z + q.w * q.y)
                mo[7] = 2.0 * (q.y * q.z + q.w * q.x)
                mo[8] = 1.0 - 2.0 * (q.x * q.x + q.y * q.y)
 
                return mo 

            else:
                print("Invalid trans_type!")
   
    def quaternion_set_rot_zxis(self, axis, theta):
        #assert((vectorMag(axis) - 1.0f) < 0.01f);
        thetaOver2 = theta * .5
        sinThetaOver2 = math.sin(thetaOver2)
       
        w = math.cos(thetaOver2)
        x = axis.x * sinThetaOver2
        y = axis.y * sinThetaOver2
        z = axis.z * sinThetaOver2

    def quaternion_get_rot_angle(self):
        thetaOver2 = math.acos(self.w)
        return thetaOver2 * 2.0
 
    def quaternion_get_rot_axis(self):
        sin_theta_over2Sq = 1.0 - self.w * self.w
        one_over_sin_theta = 1.0 / math.sqrt(sin_theta_over2Sq)
        
        nx = self.x * one_over_sin_theta
        ny = self.y * one_over_sin_theta
        nz = self.z * one_over_sin_theta
        
        return vec3(nx, ny, nz)
 

    def quaternion_slerp (self, q0, q1, t):
 
        if (t <= 0):
            return q0
        if (t >= 1):
            return q1

        cosOmega = self.dot_product(q0, q1)
        
        q1w = q1.w
        q1x = q1.x
        q1y = q1.y
        q1z = q1.z
        
        # avoid getting different results
        if (cosOmega < 0):
            q1w = -q1w
            q1x = -q1x
            q1y = -q1y
            q1z = -q1z
            cosOmega -= cosOmega
        
        k0 = 0
        k1 = 0
        # avoid sth over 0 happening
        if cosOmega > 0.999:
            k0 = 1.0 - t
            k1 = t
        else:
        
            sinOmega = math.sqrt(1.0 - cosOmega * cosOmega)
            omega = math.atan2 (sinOmega, cosOmega)
            oneOverSinOmega = 1.0/sinOmega
            
            k0 = math.sin ((1.0- t) * omega) * oneOverSinOmega
            k1 = math.sin (t*omega) * oneOverSinOmega
        
        ####
        result = type(self)()
        
        result.x = k0 * q0.x + k1 * q1x;
        result.y = k0 * q0.y + k1 * q1y;
        result.z = k0 * q0.z + k1 * q1z;
        result.w = k0 * q0.w + k1 * q1w;
        
        return result

*/

/**********************************************************************/


/*
class spherical(object):
    def __init__(self,r=0,t=0,p=0):
        """ r = radius 
            t = theta 
            p = phi  (not needed for polar)
        """
        #self.mu = math_util() 
        self.r=r
        self.t=t
        self.p=p

    def __repr__(self):
        return '(%s, %s, %s)' % (self.r, self.p, self.t)

    def __getitem__(self, index):
        if index==0:
            return self.r
        if index==1:
            return self.t
        if index==2:
            return self.p

    def __setitem__(self, key, item):
        if key==0:
            self.r = item
        if key==1:
            self.t = item
        if key==2:
            self.p = item

    def to_cartesian(self, deg_rad='rad'):
        x = self.r * math.sin(self.p) * math.cos(self.t)
        y = self.r * math.sin(self.p) * math.sin(self.t)
        z = self.r * math.cos(self.p)    
        return (x,y,z)
 

    //  UNTESTED 
    //  from David Gould's book
    //  Complete Maya Programing II 
    //  page 14 
    //  r = length( x y z )
    //  p = tan-1 (length(x y), z)) 
    //  t = tan-1 (y,x)
    
    // def from_cartesian(self, vec3):
    //     if isinstance(vec3,tuple):
    //         vec3 = vec3(vec3[0],vec3[1],vec3[2]) 
    //     r = vec3.length  
    //     p = math.atan( -1*math.sqrt(vec3[0]*vec3[0]+vec3[1]*vec3[1])*vec3[2]  ) 
    //     t = math.atan( -1*(vec3[1]*vec3[0]))
    //     print('### r %s p %s t %s '%(r,p,t))
    //     self.r=r
    //     self.t=t
    //     self.p=p
    //     #return type(self)(r,p,t)


    //UNTESTED
    //polar coordinates are spherical without the phi element 
    //from David Gould's book
    //Complete Maya Programing II 
    //page 13         
    
    // void cartesian_to_polar (vec3){
    //     if isinstance(vec3,tuple):
    //         vec3 = vec3(vec3[0],vec3[1],0) #Z is ignored  
    //     r = math.sqrt(vec3[0]*vec3[0] + vec3[1]*vec3[1])  
    //     t = math.atan( vec3[1]*vec3[0] ) 
    //     print('### cartesian to polar  r %s t %s '%(r,t))
    //     self.r=r
    //     self.t=t
    //     self.p=0
    // }

*/


/*****************************/

/*



    def matrix_invert(self, aMatrix, size):
        
        """
        # From the brilliant Bryant Ewert's script 
        matrix_playground.mel 

        # Returns a matrix which is the Inverse of $aMatrix.
        # The $size of the input matrix must be specified (3 or 4).

        """

        iMatrix  = matrix44()
    
        #determinant = determinant( aMatrix, size )
    
        if  determinant != 0.0: 
            #iMatrix = ( 1 / determinant( aMatrix, size ) ) * adjoint( aMatrix, size )
            pass

        return iMatrix;


    def matrix_rotate(self, aMatrix, size):
        
        """
        # From the brilliant Bryant Ewert's script 
        matrix_playground.mel 

        # Applies a Rotation Transformation to the specified Matrix ("A", "B" or "C").
        # The rotation value and unit is derived from the current UI settings.
        # Note: The rotation matrices used may seem Transposed to those typically
        #    documented, but they are correct for this implementation within Maya,
        #    specifically in regard to the Acquire and Apply functions (above).

        """

    
        sAxis = ( "X", "Y", "Z" )
        kRADIAN = 57.295779513082320876798154814105;
        aMatrix = matrix44()
        rMatrix = matrix44() 
  
*/


/*
class vec4(object):
    """ untested - 
        homogeneous coordinate experiment  
    """

    def __init__(self,x=0,y=0,z=0,w=1):
        self.x=x
        self.y=y
        self.z=z
        self.w=w  
        
        #self.mu = math_util() 

    def __mul__(self, other):
        # https://www.tomdalling.com/blog/modern-opengl/explaining-homogenous-coordinates-and-projective-geometry/
        
        if isinstance(other, float) or isinstance(other, int):
            #return type(self)(self.x*other, self.y*other, self.z*other)
            
            new_x = self.x * other 
            new_y = self.y * other 
            new_z = self.z * other 
            new_w = self.w * other 
            return type(self)( (new_x/new_w), (new_y/new_w), (new_z/new_w), new_w )


    def __repr__(self):
        return '(%s, %s, %s, %s)' % (self.x, self.y, self.z, self.w)

    def __getitem__(self, index):
        if index==0:
            return self.x
        if index==1:
            return self.y
        if index==2:
            return self.z
        if index==3:
            return self.w

    def __setitem__(self, key, item):
        if key==0:
            self.x = item
        if key==1:
            self.y = item
        if key==2:
            self.z = item
        if key==3:
            self.w = item

    def insert(self, iterable):
        """ convert an np.array, tuple or list  to vec3  
            does not check size, so just assume 3 items (x,y,z)
        """
        if isinstance(iterable, vec3):
            self.from_vec3(iterable) 

        if isinstance(iterable, list) or isinstance(iterable, tuple):
            self.x = iterable[0]
            self.y = iterable[1]            
            self.z = iterable[2]

            if len(iterable)==3: 
                self.w = 1  
            if len(iterable)==4:
                self.w = iterable[3]                 

        return self 

    def to_vec3(self):
        """ vec4 to vec3 - 
            divide the weight across the other 3 axis 
        """
        w = self.w
        return vec3( (self.x/w), (self.y/w), (self.z/w) )

    def from_vec3(self, vec3):
        """ vec4 from a vec3, simply add a weight of 1 """
        self.x = vec3.x        
        self.y = vec3.y  
        self.z = vec3.z
        self.w = 1  

#[xyzw]∗⎡⎣⎢⎢⎢m00m10m20m30m01m11m21m31m02m12m22m32m03m13m23m33⎤⎦⎥⎥⎥
#x′=x∗m00+y∗m10+z∗m20+w∗m30y′=x∗m01+y∗m11+z∗m21+w∗m31z′=x∗m02+y∗m12+z∗m22+w∗m32w′=x∗m03+y∗m13+z∗m23+w∗m33



###############################################
class matrix44(object):
    """ 4X4 matrix from pure python 
        limited support to interface to numpy arrays 

        the patern "return type(self) is nice to return copies of itself,
        but beware that this structure is not compatible for passing mutable types.
        Only primitive types work, in this case floats  


       -------------------------------------------------------------------------
       standard affine transformation matrix.

       ⎡m00  m01 m02 0⎤
       ⎢m10  m11 m12 0⎥
       ⎢m20  m21 m22 0⎥
       ⎣Tx   Ty  Tz  1⎦
       -------------------------------------------------------------------------

    """    

    def __init__(self, a=1,b=0,c=0,d=0,
                       e=0,f=1,g=0,h=0,
                       i=0,j=0,k=1,l=0,
                       m=0,n=0,o=0,p=1):

        self.mu = math_util()
        self.m = [a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p]

    def __getitem__(self, index):
        return self.m[index]

    def __setitem__(self, key, item):
        self.m[key] = item

    def __repr__(self):
        return '(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s )'%(
                self.m[0], self.m[1], self.m[2],  self.m[3],  self.m[4],  self.m[5],  self.m[6],  self.m[7], 
                self.m[8], self.m[9], self.m[10], self.m[11], self.m[12], self.m[13], self.m[14], self.m[15])

    def __add__(self, n):
        return type(self)(
                self.m[0] +n[0] , self.m[1]+n[1]  , self.m[2]+n[2]  , self.m[3]+n[3]  , 
                self.m[4] +n[4] , self.m[5]+n[5]  , self.m[6]+n[6]  , self.m[7]+n[7]  ,
                self.m[8] +n[8] , self.m[9]+n[9]  , self.m[10]+n[10], self.m[11]+n[11],
                self.m[12]+n[12], self.m[13]+n[13], self.m[14]+n[14], self.m[15]+n[15]
               )
       
    def __sub__(self, n):
        return type(self)(
                self.m[0] -n[0] , self.m[1]-n[1]  , self.m[2]-n[2]  , self.m[3]-n[3]  , 
                self.m[4] -n[4] , self.m[5]-n[5]  , self.m[6]-n[6]  , self.m[7]-n[7]  ,
                self.m[8] -n[8] , self.m[9]-n[9]  , self.m[10]-n[10], self.m[11]-n[11],
                self.m[12]-n[12], self.m[13]-n[13], self.m[14]-n[14], self.m[15]-n[15]
               )
   
    def __mul__(self, n):
        """multiply this 4X4 by another 4X4 matrix or a vec3 vector4 """

        if isinstance(n, vec4):
            #untested
            #https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic
            #    -projection-matrix/building-basic-perspective-projection-matrix

            # row major * vec4 
            # outx = n.x*self.m[0]   + n.y*self.m[1]  +  n.z*self.m[2]   +  n.w *self.m[3] 
            # outy = n.x*self.m[4]   + n.y*self.m[5]  +  n.z*self.m[6]   +  n.w *self.m[7] 
            # outz = n.x*self.m[8]   + n.y*self.m[9]  +  n.z*self.m[10]  +  n.w *self.m[11]                         
            # outw = n.x*self.m[12]  + n.y*self.m[13] +  n.z*self.m[14]  +  n.w *self.m[15] 
            
            # column major * vec4 
            outx = n.x*self.m[0]  + n.y*self.m[4]  +  n.z*self.m[8]   +  n.w *self.m[12] 
            outy = n.x*self.m[1]  + n.y*self.m[5]  +  n.z*self.m[9]   +  n.w *self.m[13] 
            outz = n.x*self.m[2]  + n.y*self.m[6]  +  n.z*self.m[10]  +  n.w *self.m[14]                         
            outw = n.x*self.m[3]  + n.y*self.m[7]  +  n.z*self.m[11]  +  n.w *self.m[15] 

            return  (outx, outy, outz, outw)



            # column major  , without elements 12,13,14 
            outx = self.m[0] * n.x + self.m[4] * n.y + self.m[8]  * n.z 
            outy = self.m[1] * n.x + self.m[5] * n.y + self.m[9]  * n.z 
            outz = self.m[2] * n.x + self.m[6] * n.y + self.m[10] * n.z 
            return  (outx, outy, outz)


        if isinstance(n, tuple) or isinstance(n, list):
            
            # what is the purspose of adding 12,13,14 ?
            outx = self.m[0] * n[0] + self.m[4] * n[1] + self.m[8]  * n[2]     + self.m[12]
            outy = self.m[1] * n[0] + self.m[5] * n[1] + self.m[9]  * n[2]     + self.m[13]
            outz = self.m[2] * n[0] + self.m[6] * n[1] + self.m[10] * n[2]     + self.m[14]

            # column major, same as first, without 12,13,14               
            #outx = self.m[0] * n[0] + self.m[4] * n[1] + self.m[8]  * n[2] 
            #outy = self.m[1] * n[0] + self.m[5] * n[1] + self.m[9]  * n[2] 
            #outz = self.m[2] * n[0] + self.m[6] * n[1] + self.m[10] * n[2] 

            # row major - same as first, without 12,13,14    
            # outx = self.m[0] * n[0] + self.m[1] * n[1] + self.m[2]  * n[2] 
            # outy = self.m[4] * n[0] + self.m[5] * n[1] + self.m[6]  * n[2] 
            # outz = self.m[8] * n[0] + self.m[9] * n[1] + self.m[10] * n[2] 
            return  (outx, outy, outz)


        if type(n) == type(self):
            return type(self)(
                    self.m[0]*n[0]  + self.m[1]*n[4]  + self.m[2]*n[8]   + self.m[3]*n[12] ,
                    self.m[0]*n[1]  + self.m[1]*n[5]  + self.m[2]*n[9]   + self.m[3]*n[13] ,
                    self.m[0]*n[2]  + self.m[1]*n[6]  + self.m[2]*n[10]  + self.m[3]*n[14] ,
                    self.m[0]*n[3]  + self.m[1]*n[7]  + self.m[2]*n[11]  + self.m[3]*n[15] ,
                    self.m[4]*n[0]  + self.m[5]*n[4]  + self.m[6]*n[8]   + self.m[7]*n[12] ,
                    self.m[4]*n[1]  + self.m[5]*n[5]  + self.m[6]*n[9]   + self.m[7]*n[13] ,
                    self.m[4]*n[2]  + self.m[5]*n[6]  + self.m[6]*n[10]  + self.m[7]*n[14] ,
                    self.m[4]*n[3]  + self.m[5]*n[7]  + self.m[6]*n[11]  + self.m[7]*n[15] ,
                    self.m[8]*n[0]  + self.m[9]*n[4]  + self.m[10]*n[8]  + self.m[11]*n[12],
                    self.m[8]*n[1]  + self.m[9]*n[5]  + self.m[10]*n[9]  + self.m[11]*n[13],
                    self.m[8]*n[2]  + self.m[9]*n[6]  + self.m[10]*n[10] + self.m[11]*n[14],
                    self.m[8]*n[3]  + self.m[9]*n[7]  + self.m[10]*n[11] + self.m[11]*n[15],
                    self.m[12]*n[0] + self.m[13]*n[4] + self.m[14]*n[8]  + self.m[15]*n[12],
                    self.m[12]*n[1] + self.m[13]*n[5] + self.m[14]*n[9]  + self.m[15]*n[13],
                    self.m[12]*n[2] + self.m[13]*n[6] + self.m[14]*n[10] + self.m[15]*n[14],
                    self.m[12]*n[3] + self.m[13]*n[7] + self.m[14]*n[11] + self.m[15]*n[15]
                   )

    @property
    def identity(self):
        """ using the "standard" 16 element, Row major, 4X4 rotation matrix """
        #return  [ 1,0,0,0 ,0,1,0,0 ,0,0,1,0 ,0,0,0,1]
        return type(self)()

    def np_inverse(self, mtype='numpy'):
        """ untested """
        if NUMPY_IS_LOADED:
            a = self.copy(mtype='numpy')
            b = inv(a)
            if mtype=='numpy':
                return b 
            if mtype=='m44':    
                c = matrix44()
                c.insert(b)
                return c

    #@property
    def test_index(self):
        """ fill matrix with incrementing number to see indices """
        tmp = type(self)()
        for i in range(16):
            tmp[i] = i
        return tmp

    def serialize(self, inarray):
        """ serialize this array into a list """

        out = []
        for i in self.m:
            out.append(i)
        return out

    def insert(self, iterable):
        """ UNTESTED - load the first 16 things we find into this matrix """

        if isinstance(iterable, matrix44):
            self.m = iterable.m

        #serialized simple array
        if isinstance(iterable, list) or isinstance(iterable, tuple):
            for idx,i in enumerate(iterable):
                if idx <= 16:
                     self.m[idx] = iterable[idx] 


    #@property
    def copy(self, mtype=None):
        """ UNTESTED """
        if mtype == None:        
            return type(self)(
                self.m[0] , self.m[1] , self.m[2] , self.m[3] ,
                self.m[4] , self.m[5] , self.m[6] , self.m[7] ,
                self.m[8] , self.m[9] , self.m[10], self.m[11],
                self.m[12], self.m[13], self.m[14], self.m[15]
            )

         

    @property
    def transpose(self):
        return type(self)(
            self.m[0], self.m[4], self.m[8] , self.m[12],
            self.m[1], self.m[5], self.m[9] , self.m[13],
            self.m[2], self.m[6], self.m[10], self.m[14],
            self.m[3], self.m[7], self.m[11], self.m[15]
        )

    @property
    def determinant(self):
        """
            Laplace expansion method 

            https://www.mathsisfun.com/algebra/matrix-determinant.html

            plus  a times the determinant of the matrix that is not in a's row or column,
            minus b times the determinant of the matrix that is not in b's row or column,
            plus  c times the determinant of the matrix that is not in c's row or column,
            minus d times the determinant of the matrix that is not in d's row or column,

                       |               | A*        -     B*      +        C*    -         D*|
            a  b  c  d | 0   1  2   3  | a -->     |  <--b-->    |     <--c-->  |      <--d |  
            e  f  g  h | 4   5  6   7  | |  f g h  |  e  |  g  h |  e  f  |  h  | e  f  g | |
            i  j  k  l | 8   9  10  11 |    j k l  |  i     k  l |  i  j     l  | i  j  k   |
            m  n  o  p | 12  13 14  15 |    n o p  |  m     o  p |  m  n     p  | m  n  o   | 

        """  


        def det33(a):
            #same as matrix33 determinant method 
            o = a[0]* ((a[4]*a[8])-(a[5]*a[7])) - a[1]*((a[3]*a[8])-(a[5]*a[6])) +  a[2]*((a[3]*a[7])-(a[4]*a[6]))
            return o

        a = self.copy() 
        
        a33 = [a[5],a[6],a[7],a[9],a[10],a[11],a[13],a[14],a[15] ]
        b33 = [a[4],a[6],a[7],a[8],a[10],a[11],a[12],a[14],a[15] ]
        c33 = [a[4],a[5],a[7],a[8],a[9] ,a[11],a[12],a[13],a[15] ]
        d33 = [a[4],a[5],a[6],a[8],a[9] ,a[10],a[12],a[13],a[14] ]
        o = (a[0] * det33(a33)) - (a[1]*det33(b33)) + (a[2]*det33(c33)) - (a[3]*det33(d33))
        return o

    def batch_mult_pts(self, pts):
        """ sub component of matrix rotate function 
            multiply a 4X4 matrix by a list of points 
        """
        #debug - make work with other types??

        tmp_buffer = []
        out = None
        for pvec in pts:  
            #total experiment for perpective/homogeneous coordinates  
            #pvec = vec4(pt[0], pt[1], pt[2], 1)
            #print(pvec)
            tmp_buffer.append( self * pvec )
 
        return tmp_buffer

    def from_euler(self, xrot, yrot, zrot):
        """
            derived from the rotate_pts_3d function 
            go read that doc for explanation  
        """
        dtr = self.mu.dtr

        ####
        #build rotationY (see diagram above) 
        y_matrix     =  self.identity
        y_matrix[0]  =  math.cos(dtr( yrot ))
        y_matrix[2]  = -math.sin(dtr( yrot ))
        y_matrix[8]  =  math.sin(dtr( yrot ))
        y_matrix[10] =  math.cos(dtr( yrot ))

        ####                
        #build rotationZ (see diagram above) 
        z_matrix    =  self.identity
        z_matrix[0] =  math.cos(dtr( zrot ))
        z_matrix[1] =  math.sin(dtr( zrot ))
        z_matrix[4] = -math.sin(dtr( zrot ))
        z_matrix[5] =  math.cos(dtr( zrot ))
        tmp_matr = y_matrix * z_matrix 

        ####
        #build rotationX (see diagram above) 
        x_matrix     =  self.identity
        x_matrix[5]  =   math.cos(dtr( xrot )) 
        x_matrix[6]  =   math.sin(dtr( xrot )) 
        x_matrix[9]  =  -math.sin(dtr( xrot ))
        x_matrix[10] =   math.cos(dtr( xrot ))
        rotation_44 = x_matrix * tmp_matr
 
        self.insert(rotation_44)

    def rotate_pts_3d(self, points, xrot, yrot, zrot):
        """
           The "standard" 16 element, Row major, 4X4 rotation matrix 

           ⎡0   1   2   3  ⎤    ⎡   XVEC    0 ⎤
           ⎢4   5   6   7  ⎥    ⎢   YVEC    0 ⎥
           ⎢8   9   10  11 ⎥    ⎢   ZVEC    0 ⎥
           ⎣12  13  14  15 ⎦    ⎣ 0  0  0   0 ⎦

           ⎡0   1   2   3 ⎤     ⎡xx  xy  xz  0⎤
           ⎢4   5   6   7 ⎥     ⎢yx  yy  yz  0⎥
           ⎢8   9   10  11⎥     ⎢zx  zy  zz  0⎥
           ⎣12  13  14  15⎦     ⎣0   0   0   0⎦
           ------------------------------
           rotate Y matrix     
           ⎡  cos(y)  0      -sin(y)  0 ⎤ 
           ⎢  0       1       0       0 ⎥ 
           ⎢  sin(y)  0       cos(y)  0 ⎥ 
           ⎣  0       0       0       1 ⎦
           ------------------------------
           rotate Z  matrix 
           ⎡  cos(z)  sin(z)  0       0 ⎤ 
           ⎢ -sin(z)  cos(z)  0       0 ⎥
           ⎢  0       0       1       0 ⎥
           ⎣  0       0       0       1 ⎦
           ------------------------------
           rotate X matrix  
           ⎡  1       0       0       0 ⎤  
           ⎢  0       cos(x)  sin(x)  0 ⎥  
           ⎢  0      -sin(x)  cos(x)  0 ⎥  
           ⎣  0       0       0       1 ⎦ 
        """
        dtr = self.mu.dtr

        ####
        #build rotationY (see diagram above) 
        y_matrix     =  self.identity
        y_matrix[0]  =  math.cos(dtr( yrot ))
        y_matrix[2]  = -math.sin(dtr( yrot ))
        y_matrix[8]  =  math.sin(dtr( yrot ))
        y_matrix[10] =  math.cos(dtr( yrot ))

        ####                
        #build rotationZ (see diagram above) 
        z_matrix    =  self.identity
        z_matrix[0] =  math.cos(dtr( zrot ))
        z_matrix[1] =  math.sin(dtr( zrot ))
        z_matrix[4] = -math.sin(dtr( zrot ))
        z_matrix[5] =  math.cos(dtr( zrot ))
        tmp_matr = y_matrix * z_matrix 

        ####
        #build rotationX (see diagram above) 
        x_matrix     =  self.identity
        x_matrix[5]  =   math.cos(dtr( xrot )) 
        x_matrix[6]  =   math.sin(dtr( xrot )) 
        x_matrix[9]  =  -math.sin(dtr( xrot ))
        x_matrix[10] =   math.cos(dtr( xrot ))
        rotation_44 = x_matrix * tmp_matr

        ############ 
        return rotation_44.batch_mult_pts(points)

    def buildPerspProjMat(self, fov, aspect, znear, zfar):
        """
            UNTESTED 
            
            transformation matrix that changes the W element of each vertex. 
            After the the camera matrix is applied to each vertex, 
            but before the projection matrix is applied, 
            the Z element of each vertex represents the distance away from the camera. 
            the larger Z is, the more the vertex should be scaled down


            http://stackoverflow.com/questions/8633034/basic-render-3d-perspective-projection-onto-2d-screen-with-camera-without-openg

            use homogenous transformations and coordinates. 

            take a point in space and:
                Position it relative to the camera using the model matrix.
                Project it either orthographically or in perspective using the projection matrix.
                Apply the viewport transformation to place it on the screen.




        """

        PI_OVER_360 = 0.00872664625
        #xymax = 100
        xymax = znear * math.tan(fov * PI_OVER_360);

        ymin = -xymax
        xmin = -xymax

        width = xymax - xmin
        height = xymax - ymin

        depth =  zfar - znear
        
        #avoid those pesky divide by zero errors 
        if depth==0:
            depth=.1
        if width==0:
            width=1
        if height==0:
            height=1                        

        q     = -(zfar + znear) / depth
        qn    = -2 * (zfar * znear) / depth

        w = 2 * znear / width
        w = w / aspect
        h = 2 * znear / height

        m = self.identity

        # m[0]  = w; m[4]  = 0; m[8]  = 0 ; m[12] = 0
        # m[1]  = 0; m[5]  = h; m[9]  = 0 ; m[13] = 0
        # m[2]  = 0; m[6]  = 0; m[10] = q ; m[14] = qn 
        # m[3]  = 0; m[7]  = 0; m[11] =-1 ; m[15] = 0

        m[0]=w ; m[1]=0  ; m[2]=0  ; m[3]=0;
        m[4]=0 ; m[5]=h  ; m[6]=0  ; m[7]=0;
        m[8]=0 ; m[9]=0  ; m[10]=q ; m[11]=-1;
        m[12]=0; m[13]=0 ; m[14]=qn; m[15]=0;
        
        #print(m)
        return m

*/


