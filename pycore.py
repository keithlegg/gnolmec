
import sys


from pygfx.math_ops import  *
from pygfx.point_ops import *
from pygfx.obj3d import  *
from pygfx.render import *

mu = math_util() 



##***********************************************************##

## print(sys.argv, len(sys.argv))

if __name__=="__main__":

    print( sys.argv )


    PYCORE_OBJ_IN   = sys.argv[1]
    PYCORE_GEOMPATH = "3d_obj"
    PYCORE_OBJ_OUT  = "%s/%s"%(PYCORE_GEOMPATH, "PYCORE.obj")

    # print("# PYCORE %s --> %s "% (PYCORE_OBJ_IN, PYCORE_OBJ_OUT) )


##***********************************************************##
##***********************************************************##
## DEFINE PYCORE COMMANDS (FROM PYGFX)

def XXtriangulate():
    obj = object3d()
    obj.load(PYCORE_OBJ_IN)
    obj.triangulate()
    obj.save(PYCORE_OBJ_OUT)


##------------------

def triangulate():
    obj = object3d() 
    obj.load(PYCORE_OBJ_IN)

    vectors = [] 

    # iterate all faces, calculate a normal for it 
    # transfrom that normal vector to the 3D center of the face 
    for fid in range(len(obj.polygons)):
        nrml = obj.get_face_normal(fid) 
        cntr = obj.get_face_centroid(fid)
        vectors.append( (nrml, cntr) )

    obj2 = object3d()    
    obj2.vectorlist_to_obj(vectors)
    obj2.save(PYCORE_OBJ_OUT) 

##------------------



##------------------



##***********************************************************##
##***********************************************************##
## PARSE PYCORE COMMANDS 

if __name__=="__main__":

    if sys.argv[2] == 'triangulate':
        triangulate()
    
    if sys.argv[2] == 'normals':
        triangulate()









###########################################################################
###########################################################################


def build_orthogonal_vector():
    """ treats the "line" as an infinite vector 

    """

    obj = object3d()
    com = vec3() #container for commands

    # the point we are "looking" from 
    pt1 = vec3(-1,1,-4)
    obj.prim_cube(pos=pt1,size=.05,linecolor=(255,0,0),rot=(0,0,0),pivot='world')

    # the point of the line origin
    pt2 = vec3(10,-5, 17)
    obj.prim_cube(pos=pt2,size=.1,linecolor=(255,0,0),rot=(0,0,0),pivot='world')    
    
    # the line, needs to be normalized for the math to work  
    display_unitvec = vec3(0,1,0)
    unitvec = display_unitvec.normal
    
    #render it as full size, not unit length 
    obj.one_vec_to_obj( display_unitvec , pos=pt2) 
    #make a negative version as well, to really get the idea of the size 
    display_unitvec = display_unitvec * -1
    obj.one_vec_to_obj( display_unitvec , pos=pt2) 

    d = com.orthogonal_vec_from_pt(pt2, unitvec, pt1)

    #obj.one_vec_to_obj( d*-1 )   
    obj.one_vec_to_obj( d , pt1 )   

    obj.save('3d_obj/normals.obj')


#build_orthogonal_vector() 
