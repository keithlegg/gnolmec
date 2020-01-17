
import sys 

# from gnelscript import examples
# from gnelscript import examples_2d
# from gnelscript import examples_wip
# from gnelscript import examples_milling
# from gnelscript import examples_raster

from gnelscript import examples_vector as ev 

# from gnelscript import examples_selection




if __name__=="__main__":
    PYCORE_OBJ_IN   = sys.argv[1]
    PYCORE_GEOMPATH = "3d_obj"
    PYCORE_OBJ_OUT  = "%s/%s"%(PYCORE_GEOMPATH, "PYCORE.obj")
    PYCORE_BMP_OUT  = "py_render.bmp"
    M44_DISK_FILE = "camera_matrix.olm"
    # print("# PYCORE %s --> %s "% (PYCORE_OBJ_IN, PYCORE_OBJ_OUT) )



#ev.make_normal_all_faces(PYCORE_OBJ_IN, PYCORE_OBJ_OUT ) 


"""
from gnelscript import examples_raster

examples_raster.vectorize('snow.jpg')

"""



