#ifndef MATHOP_H    
#define MATHOP_H


//degree = radian * (180 / PI) # PI = 3.14159265
#define DEG_TO_RAD 0.0174532925 
//radian = degree * (PI/180)
#define RAD_TO_DEG 57.29577951  


//2 dimensional vector 
struct vector2d {
    float x;
    float y;
};

//3 dimensional vector 
struct vector3d {
    float x;
    float y;
    float z;    
};


double deg_to_rad ( double deg);
double rad_to_deg ( double rad);
float dotProduct ( vector2d v1, vector2d v2);


float fcalc_distance(int pt1[2], int pt2[2]);
float fcalc_distance(float pt1[2], float pt2[2]);
float fcalc_distance( vector2d input);
float fcalc_distance( vector3d input);


float calc_theta_vert ( float start_x, float start_y, float end_x, float end_y);
float angle_between( vector2d v_one, vector2d v_two );


//object instance generators
vector2d newvec2( double x, double y );
vector2d newvec2( int   x, int   y );

vector2d normalize( vector2d input );
vector2d scale_vec( vector2d input, float amount );
vector2d mult_vec_scalar( vector2d input, float amount );

vector2d line2vect(int start_x, int start_y, int end_x, int end_y);
vector2d line2vect(float start_x, float start_y, float end_x, float end_y);
vector2d vmul_2d ( vector2d v1, vector2d v2 );

/////////

vector3d newvec3( double x, double y, double z );
vector3d newvec3( int   x, int   y, int   z );

vector3d normalize( vector3d input );

#endif

