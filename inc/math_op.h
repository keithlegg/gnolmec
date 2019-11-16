#ifndef MATHOP_H    
#define MATHOP_H


//degree = radian * (180 / PI) # PI = 3.14159265
#define DEG_TO_RAD 0.0174532925 
//radian = degree * (PI/180)
#define RAD_TO_DEG 57.29577951  


//2 dimensional vector 
struct vector2d {
    double x;
    double y;
};

//3 dimensional vector 
struct vector3d {
    double x;
    double y;
    double z;    
};


//2X2 matrix 
struct m22 {
    double m0; double m1;
    double m2; double m3;
};

//3X3 matrix 
struct m33 {
    double m0; double m1; double m2;    
    double m3; double m4; double m5;
    double m6; double m7; double m8;    
};


//4X4 matrix 
struct m44 {
    double m0 ; double m1 ; double m2 ; double m3;
    double m4 ; double m5 ; double m6 ; double m7;
    double m8 ; double m9 ; double m10; double m11;
    double m12; double m13; double m14; double m15; 
};


double deg_to_rad ( double deg);
double rad_to_deg ( double rad);

double dotProduct ( vector2d v1, vector2d v2);
double dotProduct ( vector3d v1, vector3d v2);

vector3d crossProduct(vector3d v1, vector3d v2);


double fcalc_distance(int pt1[2], int pt2[2]);
double fcalc_distance(double pt1[2], double pt2[2]);
double fcalc_distance( vector2d input);
double fcalc_distance( vector3d input);


double calc_theta_vert ( double start_x, double start_y, double end_x, double end_y);
double angle_between( vector2d v_one, vector2d v_two );


//object instance generators
vector2d newvec2( double x, double y );
vector2d newvec2( int   x, int   y );

vector2d normalize( vector2d input );
vector2d scale_vec( vector2d input, double amount );
vector2d mult_vec_scalar( vector2d input, double amount );

vector2d line2vect(int start_x, int start_y, int end_x, int end_y);
vector2d line2vect(double start_x, double start_y, double end_x, double end_y);
vector2d vmul_2d ( vector2d v1, vector2d v2 );

/////////

vector3d newvec3( double x, double y, double z );
vector3d newvec3( int   x, int   y, int   z );

vector3d normalize( vector3d input );

//untested - not done 
vector3d dtr_vec3(vector3d invec);
vector3d rtd_vec3(vector3d invec);       
vector3d vect_add( vector3d v1, vector3d v2);
vector3d mult_scalar (double scalar, vector3d v);

vector3d mult_m33_vec3(m33 m, vector3d v);

/////////

m33 new_m33( double m0 , double m1 , double m2,  
             double m3 , double m4 , double m5, 
             double m6 , double m7 , double m8
    );

m44 new_m44( double m0 , double m1 , double m2 , double m3, 
             double m4 , double m5 , double m6 , double m7,
             double m8 , double m9 , double m10, double m11,
             double m12, double m13, double m14, double m15
    );


#endif

