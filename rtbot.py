import os 
import sys 

from gnelscript import examples
from gnelscript import examples_2d
from gnelscript import examples_wip
from gnelscript import examples_milling
from gnelscript import examples_raster
from gnelscript import examples_vector as ev 
from gnelscript import examples_selection



 
## if __name__=="__main__":
##     PYCORE_OBJ_IN   = sys.argv[1]
##     PYCORE_GEOMPATH = "3d_obj"
##     PYCORE_OBJ_OUT  = "%s/%s"%(PYCORE_GEOMPATH, "PYCORE.obj")
##     PYCORE_BMP_OUT  = "py_render.bmp"
##     M44_DISK_FILE = "camera_matrix.olm"
##     # print("# PYCORE %s --> %s "% (PYCORE_OBJ_IN, PYCORE_OBJ_OUT) )
 


#ev.make_normal_all_faces(PYCORE_OBJ_IN, PYCORE_OBJ_OUT ) 


"""
from gnelscript import examples_raster

examples_raster.vectorize('snow.jpg')

"""





from gnelscript.pygfx.obj3d import *

OBJ = object3d()

grid = OBJ.test_data_grid(10,10,2)

#pg.print_grid( grid )

print( OBJ.get_grid_column(grid, 3) )
print( OBJ.get_grid_row(grid, 3) )



############################################################################################

""" 
# count lines of code, from here : 
# https://stackoverflow.com/questions/38543709/count-lines-of-code-in-directory-using-python

def countlines(start, lines=0, header=True, begin_start=None):
    if header:
        print('{:>10} |{:>10} | {:<20}'.format('ADDED', 'TOTAL', 'FILE'))
        print('{:->11}|{:->11}|{:->20}'.format('', '', ''))

    for thing in os.listdir(start):
        thing = os.path.join(start, thing)
        if os.path.isfile(thing):
            if thing.endswith('.py') or thing.endswith('.c') or thing.endswith('.cpp') or thing.endswith('.h'):        	
	            with open(thing, 'r') as f:
	                newlines = f.readlines()
	                newlines = len(newlines)
	                lines += newlines
	                if begin_start is not None:
	                    reldir_of_thing = '.' + thing.replace(begin_start, '')
	                else:
	                    reldir_of_thing = '.' + thing.replace(start, '')
	                print('{:>10} |{:>10} | {:<20}'.format(
	                        newlines, lines, reldir_of_thing))
    for thing in os.listdir(start):
        thing = os.path.join(start, thing)
        if os.path.isdir(thing):
            lines = countlines(thing, lines, header=False, begin_start=start)
    return lines
#countlines('./')
""" 


