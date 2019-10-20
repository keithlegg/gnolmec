TARGET   = semraster


INCDIR   =inc
SRCDIR   =src
OBJDIR   =obj
BINDIR   =bin


CC       = g++

# compiling flags  
CFLAGS   = -I$(INCDIR) -Wall  

LINKER   = g++
# linking flags 
LFLAGS   = -Wall -lm -lglut -lGL -lGLU -lX11 -lXi  -lm -lz

SOURCES  := $(wildcard $(SRCDIR)/*.c)

OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

# #LINUX
# #g++ $(OBJ) -o semraster $(INC) $(MOAR) -lX11 -lXi -lglut -lGL -lGLU -lm -lz -lpng 
# #OSX
# #g++ $(OBJ) -o semraster $(INC) $(MOAR) -lm -lz -framework Glut -framework OpenGL

.PHONY: clean
clean:
	@$(rm) $(TARGET)
	@$(rm) $(OBJECTS)
	@echo "cleanup done"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
	 
