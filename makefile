# final project: Kelley Kelley
EXE=final

# Main target
all: $(EXE)

#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lfreeglut -lmingw32 -lSDL2main -lSDL2 -mwindows -lSDL2_mixer -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'|tr 12 21)
CFLG=-O3 -Wall -Wno-deprecated-declarations -DRES=$(RES)
LIBS=-framework GLUT -lSDL2main -lSDL2 -lSDL2_mixer -framework Cocoa -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lSDL2 -lSDL2_mixer -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

#dependencies
glututility.o: glututility.c
displayfunc.o: displayfunc.c
perlin.o: perlin.c
lehmer.o: lehmer.c
tree.o: tree.c
audio.o: audio.c
particleengine.o: particleengine.c

# Compile rules
.c.o:
	gcc -c $(CFLG)  $<
.cpp.o:
	g++ -c $(CFLG)  $<

# Link
final:final.o glututility.o displayfunc.o perlin.o lehmer.o tree.o audio.o particleengine.o
	gcc $(CFLG) -o $@ $^ $(LIBS)

# Clean
clean:
	$(CLEAN)
