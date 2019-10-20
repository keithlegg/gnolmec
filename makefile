TARGET   = semraster

CC       = g++
# compiling flags here -std=c99
CFLAGS   = -Wall -I. 

LINKER   = g++
# linking flags here
LFLAGS   = -Wall -I. -lm -lglut -lGL -lGLU -lX11 -lXi  -lm -lz

#INCLUDE  =include
SRCDIR   =src
OBJDIR   =obj
BINDIR   =bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"





##################################
## C_FLAGS := -g -Wall -Wextra
## CC := gcc
## RM := rm
## LINKFLAGS := -lX11 -lXi -lglut -lGL -lGLU -lm -lz 
## 
## .PHONY: $(TARGET)
## .PHONY: clean
## 
## VPATH:= ./src/ ./obj/ ./include/
## 
## # Path for .c , .h and .o Files 
## SRC_PATH := ./src/
## OBJ_PATH := ./obj/
## INC_PATH := -I ./include
## 
## # Executable Name 
## TARGET := exe
## 
## # Files to compile
## OBJ1 := main.o
## 
## OBJ := $(patsubst %,$(OBJ_PATH)%,$(OBJ1))
## 
## # Build .o first
## $(OBJ_PATH)%.o: $(SRC_PATH)%.c
## 	@echo [CC] $<
## 	@$(CC) $(C_FLAGS) -o $@ -c $< $(INC_PATH)                  
## 
## # Build final Binary
## $(TARGET):      $(OBJ)
## 	 @echo [INFO] Creating Binary Executable [$(TARGET)]
## 	@$(CC) -o $@ $^ $(LINKFLAGS)
## 
## # Clean all the object files and the binary
## clean:   
## 	@echo "[Cleaning]"
## 	@$(RM) -rfv $(OBJ_PATH)*
## 	@$(RM) -rfv $(TARGET)
## 
## 
## #LINUX
## #g++ $(OBJ) -o semraster $(INC) $(MOAR) -lX11 -lXi -lglut -lGL -lGLU -lm -lz -lpng 
## #OSX
## #g++ $(OBJ) -o semraster $(INC) $(MOAR) -lm -lz -framework Glut -framework OpenGL




