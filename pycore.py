
import sys


from gnelscript.pygfx.math_ops import  *
from gnelscript.pygfx.point_ops import *
from gnelscript.pygfx.obj3d import  *
from gnelscript.pygfx.render import *

from gnelscript.pygfx.kicad_ops import * 
from gnelscript.pygfx.milling_ops import * 

mu = math_util() 



##***********************************************************##

## print(sys.argv, len(sys.argv))

if __name__=="__main__":

    # print( sys.argv )


    PYCORE_OBJ_IN   = sys.argv[1]
    PYCORE_GEOMPATH = "3d_obj"
    PYCORE_OBJ_OUT  = "%s/%s"%(PYCORE_GEOMPATH, "PYCORE.obj")

    PYCORE_BMP_OUT  = "py_render.bmp"
    M44_DISK_FILE = "camera_matrix.olm"
    # print("# PYCORE %s --> %s "% (PYCORE_OBJ_IN, PYCORE_OBJ_OUT) )



"""
print("## DEBUG PATHS ")  
print("## PYCORE_OBJ_IN   ", PYCORE_OBJ_IN )
print("## PYCORE_GEOMPATH ", PYCORE_GEOMPATH )
print("## PYCORE_OBJ_OUT  ", PYCORE_OBJ_OUT )
print("## PYCORE_BMP_OUT  ", PYCORE_BMP_OUT )
print("## M44_DISK_FILE   ", M44_DISK_FILE )
"""


##***********************************************************##
##***********************************************************##
##***********************************************************##
##***********************************************************##
## DEFINE PYCORE COMMANDS (FROM PYGFX)




def loadkicad():
    kicad = pcbfile()

    #kicad.load_gcode('gcode/ngc/3D_Chips.ngc')

    #kicad.load_kicadpcb('gcode/kicad/sample1.kicad_pcb')
    #kicad.load_kicadpcb('gcode/kicad/zipper.kicad_pcb')
    #kicad.load_kicadpcb('gcode/kicad/simple.kicad_pcb')
    
    kicad.load_kicadpcb('gcode/kicad/cnc1.kicad_pcb')
    
    # kicad.show_geom()
    kicad.save_3d_obj(PYCORE_OBJ_OUT) 
 

def triangulate():
    obj = object3d()
    obj.load(PYCORE_OBJ_IN)
    obj.triangulate()
    obj.save(PYCORE_OBJ_OUT)


##------------------

def gen_normals():
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

def object_rotate():
    obj = object3d()
    obj.load(PYCORE_OBJ_IN)
    #pts = [(2,2,2), (4,4,4), (8,8,8)]
    pts2 = obj.rotate_pts((45,45,45) )
    #print(pts2)
    obj.save(PYCORE_OBJ_OUT)


##------------------

def copy_sop():
    #be cautious of large number of polys. It gets slow real quick!

    obj = object3d() 
    obj.load(PYCORE_OBJ_IN)
    obj.copy_sop(slice=(1,10), offset=(0,1,0), num=3, distance=.5)
    obj.save(PYCORE_OBJ_OUT)




##------------------


def modify_partial():
    obj = object3d()
    obj.load(PYCORE_OBJ_IN)
    
    geom = obj.sub_select_geom( slice=(40,120) )


    newpts = obj.points

    obj2 = object3d() 
    obj2.insert_polygons(geom[0], newpts  )      
    obj2.rotate_pts((0,45,0))
    obj2.save(PYCORE_OBJ_OUT)    



##------------------
def circle_cube_pts():

    # PYCORE_OBJ_IN not needed 
    obj = object3d()
    obj.prim_circle(axis='z', pos=(0,0,0), spokes=20, dia=.5) 
    
    #obj.scale_pts((1,5,1))
        
    obj.triangulate(force=True)


    for f in range(1,len(obj.polygons)-1,1):
        if f%3==0:
            obj.extrude_face(f, 1)
    
    #obj.scale_pts((1,5,1))
    
   # obj.rotate_pts((35,35,0))

    #obj.extrude_face(len(obj.polygons)-2, -2)

    pts = obj.get_face_pts(3) 
    ct = 0
    for pt in pts:
        tmp = object3d()
        
        #broken # tmp.prim_sphere(pos=pt, rot=(0,0,0), size=.05 )
        #tmp.prim_locator_xyz(pos=pt, rot=(0,0,0), size=1)
        #tmp.prim_cone('y', pos=pt, rot=(0,0,0), dia=.06, spokes=8)
        tmp.prim_cube(size=.1, pos=pt, rot=(ct,ct,ct), pivot='world')
        ct += 10
        obj.insert(tmp)  

    #obj.triangulate(force=True)
    obj.save(PYCORE_OBJ_OUT)

##------------------
def sphericalcoords():
    """ test of spherical coordinates to a cartesian point 
        done in a nested loop to make a sphere
       
        PYCORE_OBJ_IN not needed 
    """

    obj = object3d()

    for theta in range(-180,180,20):
        print('## theta ', theta )
        for phi in range(-180,180,20):        
            sp = spherical(1.5, mu.dtr(theta), mu.dtr(phi) ) 
            pt=  sp.to_cartesian() 
            obj.prim_cube(pos=pt, size=.1, linecolor=(255,0,0), rot=(0,0,0), pivot='world')

    obj.save(PYCORE_OBJ_OUT) 


##------------------
def procedural_1():
    obj = object3d()


    obj.prim_circle(axis='y', pos=(0,0,0), spokes=10, dia=1.2) 
    obj.triangulate(force=True)

    for i in range(1,len(obj.polygons) ):   
        obj.extrude_face(i, 5)

    for theta in range(-180,180,20):
        print('## theta ', theta )
        for phi in range(-180,180,20):        
            sp = spherical(1.5, mu.dtr(theta), mu.dtr(phi) ) 
            pt = sp.to_cartesian() 
            obj.prim_cube(pos=pt, size=.1, linecolor=(255,0,0), rot=(0,0,0), pivot='world')

    obj.scale_pts( (1,.5,1) )
    obj.rotate_pts( (0, 45, 0) )
    
    #there is NO MOVE???
    #obj.move_pts( (0,-3,0) )

    for theta in range(-180,180,20):
        print('## theta ', theta )
        for phi in range(-180,180,20):        
            sp = spherical(1.5, mu.dtr(theta), mu.dtr(phi) ) 
            pt = sp.to_cartesian() 
            obj.prim_cube(pos=pt, size=.1, linecolor=(255,0,0), rot=(0,0,0), pivot='world')

    obj.save(PYCORE_OBJ_OUT) 



##------------------
def primitive(primtype):
    # PYCORE_OBJ_IN not needed 

    obj = object3d() 

    position = (0,0,0)
    rotation = (0,0,0)
    size = 1 
    axis = 'z'

    do_flush = True

    
    # obj.prim_line( axis=axis, pos=position, rot=rotation, size=size)
    # obj.save(PYCORE_OBJ_OUT)
    # if do_flush:
    #     obj.flush()

    if primtype == 'triangle':
        obj.prim_triangle( axis=axis, pos=position, rot=rotation, size=size)
        obj.save(PYCORE_OBJ_OUT)
        if do_flush:
            obj.flush()

    if primtype == 'quad':
        obj.prim_quad( axis=axis, pos=position, rot=rotation, size=size)
        obj.save(PYCORE_OBJ_OUT)
        if do_flush:
            obj.flush()

    if primtype == 'circle':
        obj.prim_circle( axis=axis, pos=position, dia=size) #rot=rotation
        obj.save(PYCORE_OBJ_OUT)
        if do_flush:
            obj.flush()

    if primtype == 'sphere':
        obj.prim_sphere(  pos=position, rot=rotation, size=size)
        obj.save(PYCORE_OBJ_OUT)
        if do_flush:
            obj.flush()

    if primtype == 'locator':
        obj.prim_locator(  pos=position, rot=rotation, size=size)
        obj.save(PYCORE_OBJ_OUT)
        if do_flush:
            obj.flush()

    if primtype == 'locatorxyz':
        obj.prim_locator_xyz(  pos=position, rot=rotation, size=size)
        obj.save(PYCORE_OBJ_OUT)
        if do_flush:
            obj.flush()

    if primtype == 'cone':
        obj.prim_cone( axis=axis, pos=position, dia=size) #rot=rotation
        obj.save(PYCORE_OBJ_OUT)
        if do_flush:
            obj.flush()


##------------------

def pt_transform():

    """ example of translate, rotate, scale of raw points 
        translate tools work with "ptgroups", or raw points
    """

    obj = object3d()
    obj.load(PYCORE_OBJ_IN)

    obj.points = obj.scale_pts( (1,1,1)  , pts=obj.points )   
    
    obj.points = obj.rotate_pts((0,0,45), pts=obj.points ) 
    
    #obj.points = obj.xform_pts( (0,2,0),  pts=obj.points ) 

    obj.save(PYCORE_OBJ_OUT)


##------------------
def face_extrude():
    """ brute force test of face extrude 
        extrudes all faces in a polygon object 
        also will display a cheapo test "progress bar"
        because it can be slow 
    """

    obj = object3d()
    obj.load(PYCORE_OBJ_IN)

    #end = len(obj.polygons)
    end = 20

    for i in range(71, 190 ):   
        obj.extrude_face(i, .5)
    
    #for i in range(1,100 ):   
    #    obj.extrude_face(i, .1)

    obj.save(PYCORE_OBJ_OUT)
    

##------------------
def m33_to_vectors(m33, transpose=False):
    outvecs = []

    if transpose:
        v1 = vec3(m33[0], m33[3], m33[6])
        v2 = vec3(m33[1], m33[4], m33[7])
        v3 = vec3(m33[2], m33[5], m33[8])  
    else:
        v1 = vec3(m33[0], m33[1], m33[2])
        v2 = vec3(m33[3], m33[4], m33[5])
        v3 = vec3(m33[6], m33[7], m33[8])

    outvecs = [v1,v2,v3]
    return outvecs 

def m44_to_vectors(m44, transpose=False):

    outvecs = []

    if transpose:
        v1 = vec3(m44[0], m44[4], m44[8])
        v2 = vec3(m44[1], m44[5], m44[9])
        v3 = vec3(m44[2], m44[6], m44[10])  
    else:
        v1 = vec3(m44[0], m44[1], m44[2])
        v2 = vec3(m44[4], m44[5], m44[6])
        v3 = vec3(m44[8], m44[9], m44[10])

    outvecs = [v1,v2,v3]
    return outvecs 


##------------------
def visualize_matrix_rotation():
    """ 
        turn a 3X3 matrix into 3 vectors and render them as lines 

        Use numpy to rotate a matrix around a vector 
        rotate a matrix around a vector with a scalar angle (degrees) 
        visualize the matrix as a 3D object 
    """
    if NUMPY_IS_LOADED:
        obj  = object3d()
        m33  = matrix33()
        
        def render_multi_matrices(lom):   
            outvecs = []
            for m in lom:      
                tmp = m33_to_vectors( m )
                outvecs.extend( tmp )
            return outvecs
 
        axis = vec3(3,3,0)
       
        m1 = m33.from_vec3( axis , 45 )
              
        rendervecs = render_multi_matrices( [m1,m33] )

        # viz the axis
        rendervecs.append(axis)
        obj.vectorlist_to_obj( rendervecs )
        obj.save(PYCORE_OBJ_OUT)  
    else:
        print("ERROR NUMPY IS DISABLED ")


##------------------
def visualize_perspective_matrix():
    obj = object3d()
    obj.load(PYCORE_OBJ_IN)    
    persp_m44 = matrix44() 

    
    #create a perspective matrix 
                                       # fov, aspect, znear, zfar):
    persp_m44 = persp_m44.buildPerspProjMat( 15, 1, 1, 2)
    obj.points = obj.apply_matrix_pts(obj.points, m44=persp_m44)
    
    #visualize the matrix in the object 
    rendervecs = m44_to_vectors( persp_m44 )
    obj.vectorlist_to_obj( rendervecs )    

    obj.save(PYCORE_OBJ_OUT) 


##------------------

#def visualize_matrices_multiplied():

##------------------
#def visualize_matrices_multiplied():


##------------------

#def lathe_profile(axis, pts, numturns ):


#def loft_two_profiles(pts1, pts2 ):

##------------------


def loadgcode():
    gkod = gcode()
    
    #gkod.load_gcode('ngc/3D_Chips.ngc')
    
    gkod.load_gcode('3d_obj/arcspiral.ngc')

    gkod.show_data()

    gkod.save_3d_object(PYCORE_OBJ_OUT) 




##------------------
def scratch_obj1():

    """ build a new polygon object in memory from points 
        
        This could be used for a million things like procedural modeling,
        custom file importers, exporters., etc  
        
        PYCORE_OBJ_IN not needed since we are generating the model           
    """ 

    geom  = [[],[]]
    geom2 = [[],[]]

    obj = object3d()
    #obj.load(PYCORE_OBJ_IN)

    #add new geom and auto increment the ids

    #3 sided polygons with color
    pts = [(1,1,1, 1,0,0),(0,1,1, 1,0,0),(-1,-1,1, 1,0,1),(2,-2,1, 0,1,0)]
    polys = [(1,2,3),  (4,3,1) ]

    geom = obj.insert_polygons(polys, pts, geom=geom) 
 
    #4 sided polygons
    pts = [(4,-4.3,-3),(5.5,-2.5,-2.1),(-2,2,-4),(4,-4.2,1)]
    polys = [(1,2,3,4) ]

    geom2 = obj.insert_polygons(polys, pts, geom=geom2) 

    # use insert to add geom to object 
    obj.insert(geom) 
    obj.insert(geom2) 

    obj.save( PYCORE_OBJ_OUT )

##------------------
def scratch_obj2():
    """ another demo of a model built from coordinates
        when you use asnew_shell , you can append to other models 
    """
    obj = object3d()
 
    obj.load(PYCORE_OBJ_IN)

    #add new geom and auto increment the ids
    polys = [(1,2,3,4) ]
    pts = [(10,1,1),(0,11,1),(-1,-5,1),(2,-2,1)]
    obj.insert_polygons([], pts) 

    #add new geom and auto increment the ids
    pts = [(0,-3,-1),(2,-2,1),(3,-1,1)]
    obj.insert_polygons([], pts)

    #add polys without new points into same "shell"
    obj.insert_polygons( [(1,2,3,4,5,6,7),(1,7,2)], None, asnew_shell=False)

    #add new polygon in a new "shell" 
    obj.insert_polygons( [(1,2,3,4)], [(3,3,3), (3,-4,5), (-4,-2.5,3.1), (6.2,-2.7,8)], asnew_shell=True)

    obj.save(PYCORE_OBJ_OUT)


##***********************************************************##
##***********************************************************##
##***********************************************************##
##***********************************************************##

def pyrender_ogl():
    ropr = simple_render()
    persp_m44 = matrix44()    
    persp_m44.load_file( M44_DISK_FILE )

    obj = object3d()
    obj.load(PYCORE_OBJ_IN)
    #obj.triangulate(force=True)  

    use_perpective = True 
    if use_perpective:
        #                          # fov, aspect, znear, zfar):    
        m44_2 = persp_m44.buildPerspProjMat( 100, 1, .1, 5)
        persp_m44 =  persp_m44 * m44_2
    
    obj.points = obj.apply_matrix_pts(obj.points, m44=persp_m44)

    # ropr.render_obj((100,0,255), 1, 1, 1, 1, 1000/abs(persp_m44.m[14]), object3d=obj)

    img_op = PixelOp()   
    img_op.load('textures/generated2.bmp') 
    #img_op.load('render_thing/images/uvmap.bmp') 

    #img_op.save("foobar.bmp")

    lightpos = (0, 2 ,-1)
    ropr = simple_render()

    ##----------
    #ropr.COLOR_MODE = 'flat'
    #ropr.COLOR_MODE = 'lighted'
    ropr.COLOR_MODE = 'lightedshaded'


    ropr.SHOW_VTXS        = False
    ropr.SHOW_FACE_CENTER = False
    ropr.SHOW_EDGES       = False    

    ##----------

    ropr.scanline(obj, 1000/abs(persp_m44.m[14]), lightpos=lightpos, texmap=img_op ) 
    ropr.save_image( PYCORE_BMP_OUT )




def bezier3d():
    obj = object3d()

    start = (1 ,  0, 0)
    ctrl1 = (.5,  0, 0)
    ctrl2 = ( 0, .5, 0)
    end   = (0 ,  1, 0)
    kurve = [start, ctrl1, ctrl2, end]

    start  = (0   ,  1   , 0)
    ctrl1  = (1.5 ,  1.0   , 0)
    ctrl2  = (1   ,  1.5 , 0)
    end    = (2   ,  2   , 0)
    kurve2 =[start, ctrl1, ctrl2, end]
    
    curves = [kurve, kurve2] 
    
    obj.draw_splines( curves, drawctrls=True, drawhulls=True)

    obj.save(PYCORE_OBJ_OUT)




def lathe():
    """ needs to have the same num U and V to work """
    obj = object3d()

    # # simplest possible lathe example (must be square , num pts == num revolutions)
    # pts = [(.1,.1,0),(1,1,0),(2,2,0),(3,3,0)]
    # obj.lathe(pts, 4)


    # # using bezier curve function 
    # num = 23
    # start = (1 ,  0, 0)
    # ctrl1 = (.5,  0, 0)
    # ctrl2 = ( 0, .5, 0)
    # end   = (0 ,  1, 0)
    # curve = obj.cubic_bezier(num, start, ctrl1, ctrl2, end)
    # obj.lathe(curve, num)

    #------ 

    num = 10

    start  = (2 ,  0, 0)
    ctrl1  = (.5,  .5, 0)
    ctrl2  = (3, .5, 0)
    end    = (1 ,  1, 0)
    curve1 = [  start, ctrl1, ctrl2, end ]

    curvepts = obj.cubic_bezier(num, start, ctrl1, ctrl2, end )

    start  = (1   ,  1   , 0)
    ctrl1  = (2.5 ,  1.0   , 0)
    ctrl2  = (1   ,  1.5 , 0)
    end    = (2   ,  2   , 0)
    curve2 = [ start, ctrl1, ctrl2, end ]
  
    tmp      = obj.cubic_bezier(num, start, ctrl1, ctrl2, end )
   
    curvepts.extend(tmp)

    #obj.draw_splines( num, [curve1, curve2], drawctrls=True, drawhulls=True) 

    #obj.lathe2(curvepts, num*2)
    
    obj.lathe(curvepts, num*2)

    #for i in range(120):
    #    obj.extrude_face(i, -.4)

    #scal command kills COLOR - DOH !
    #obj.scale_pts( (1,2,1) )

    #------ 
    
    # num = 20
    # #cross_section = obj.calc_circle(pos=(1,0,0), rot=(0,180,0), dia=.5, axis='z', periodic=True, spokes=num-1)
    # cross_section = obj.calc_circle(pos=(1,0,0), dia=.5, axis='z', periodic=True, spokes=num-1)
    # cross_section.reverse()
    # obj.lathe(cross_section, num)


    obj.save(PYCORE_OBJ_OUT)
 






def test_pointgen():
    #OBJ.locate_pt_along3d( x1, y1, z1, x2, y2, z2, num):

    grid = OBJ.test_data_grid(10,10,1)

    #pg.print_grid( grid )

    print( OBJ.get_grid_column(grid, 3) )
    print( OBJ.get_grid_row(grid, 3) )


#####################################################

"""
obj = object3d()
#obj.load('gnelscript/objects/cube.obj')
obj.load(PYCORE_OBJ_IN)

#obj.rotate_pts((45,45,45))
obj.scale_pts((.5,.5,.5))

obj.save(PYCORE_OBJ_OUT)
#obj.save("%s/%s"%(PYCORE_GEOMPATH, "cube.obj"))
""" 



## parse commands coming in and run them
def runcommand():
    #scratch_obj1()
    #scratch_obj2()
    
    #bezier3d()

    #lathe()

    #visualize_matrix_rotation()

    #visualize_perspective_matrix()

    #gen_normals()

    #loadgcode()
    #loadkicad()



    #circle_cube_pts()
    
    #primitive('triangle')
    
    #procedural_1()
    #primitive('sphere')

    #face_extrude()
   
    #pt_transform()
    
    #procedural_1()
   
    #modify_partial()
    
    #triangulate()
    
    #object_rotate()
    
    #copy_sop()

    pass




if __name__=="__main__":

    if sys.argv[2] == 'runcommand':
        runcommand()

    if sys.argv[2] == 'scanline':
        pyrender_ogl()    

    #if sys.argv[2] == 'normals':
    #    gen_normals()










###########################################################################
###########################################################################


def build_orthogonal_vector():

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
