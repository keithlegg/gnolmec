OBJ = main.o
INC = -I "./"

##################################
#to add mutiple C files 
#MOAR = framebuffer.c point_op.c
#    g++ $(OBJ) -o semraster $(INC) $(MOAR)
##################################

MOAR = point_op.c framebuffer.c image_util.c

semraster:$(OBJ)
	 
	#LINUX
	g++ $(OBJ) -o semraster $(INC) $(MOAR) -lX11 -lXi -lglut -lGL -lGLU -lm -lz -lpng 
	
	#OSX
	#g++ $(OBJ) -o semraster $(INC) $(MOAR) -lm -lz -framework Glut -framework OpenGL

	rm -f $(OBJ)

main.o:
	g++ -c main.c $(INC) 


clean:
	rm -f $(OBJ) semraster
	rm -f *.o



