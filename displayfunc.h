#ifndef DISPLAYFUNC_H
#define DISPLAYFUNC_H

#include "glututility.h"
#include "perlin.h"
#include "tree.h"
#include "audio.h"
#include "particleengine.h"

// --- functions ---
// do this when nothing else to do
void idle();
// resize window
void reshape(int width, int height);
// do this when a key pressed
void key(unsigned char ch, int x, int y);
// do this when a special key is pressed
void special(int key, int x, int y);
// do this a lot
void display();
// initialize debug stuff
void initdebug();
// do the lighting stuff
void light();

// --- variables ---
//extern type name; then define in c
extern int gensize;
extern float playerpos[3];

#endif
