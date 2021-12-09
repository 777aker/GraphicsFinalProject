#ifndef PARTICLEENGINE_H
#define PARTICLEENGINE_H

#include "glututility.h"
#include "displayfunc.h"

// display calls this a lot
void dotheparticles();
// intialize some stuff
void initparticles();
// remake a particle
void initparticle(int i);
// when player moves translate all the particles positions
void moveparticles(float x, float y);

#endif