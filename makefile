TARGET   = semraster


INCDIR   =inc
SRCDIR   =src
OBJDIR   =obj
BINDIR   =bin


CC       = g++


#detect the OS (linux or OSX, dont care about windows)
UNAME := $(shell uname)



# compiling flags  
CFLAGS   = -I$(INCDIR) -Wall  

LINKER   = g++

# linking flags 
ifeq ($(UNAME), Linux)
    #LINUX
    #g++ $(OBJ) -o semraster $(INC) $(MOAR) -lX11 -lXi -lglut -lGL -lGLU -lm -lz -lpng 
	LFLAGS   = -Wall -lm -lglut -lGL -lGLU -lX11 -lXi  -lm -lz
endif

ifeq ($(UNAME), Darwin)
    #OSX
	LFLAGS   = -Wall -lm -lglut -lGL -lGLU -lX11 -lXi  -lm -lz
endif



SOURCES  := $(wildcard $(SRCDIR)/*.c)

OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(rm) $(TARGET)
	@$(rm) $(OBJECTS)
	@echo "cleanup done"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@$(rm) $(OBJDIR)/*.o
	@echo "Executable removed!"
	 
