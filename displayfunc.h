#ifndef DISPLAYFUNC_H
#define DISPLAYFUNC_H

#include "glututility.h"

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

// --- variables ---
// aspect ratio
extern double asp;
// size of world
extern double dim;
// field of view
extern int fov;
// projection type
extern int proj;

#endif