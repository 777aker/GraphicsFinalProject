#ifndef DISPLAYFUNC_H
#define DISPLAYFUNC_H

#include "glututility.h"
#include "perlin.h"

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
// initialize our grid values?
void initgrid();

// --- variables ---
//extern type name; then define in c
extern int gensize;
extern float ground[100][100];

#endif