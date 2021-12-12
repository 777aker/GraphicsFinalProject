#include "particleengine.h"

// got this from this tutorial
// http://nehe.gamedev.net/tutorial/particle_engine_using_triangle_strips/21001/
// actually really disappointing...particles are just, a bunch of triangle strips... :'(
// their turotial is dumb (sorry but it's just dissapointingly simple)
// also it wasn't very efficient so I'm changing it

// max number of particles
#define MAX_PARTICLES 1000

// particle struct
typedef struct {
	float life;
	//float fade;

	float r;
	float g;
	float b;

	float x;
	float y;
	float z;

	float yi;
	//float xi;
	//float zi;
} particle;

float gravity = 0.004;

// particle array
particle particles[MAX_PARTICLES];

// white ish color
float snow[3] = { 0.9, 0.9, 1.0 };

void dotheparticles() {
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// particle drawing loop
	for(int i = 0; i < MAX_PARTICLES; i++) {
		// color our particles
		glColor4f(particles[i].r, particles[i].g, particles[i].b, particles[i].life);
		glPointSize(2);
		// time to draw some snow
		glBegin(GL_POINTS);
		glVertex3f(particles[i].x, particles[i].y, particles[i].z);
		glEnd();
		// now move the particle
		particles[i].y += particles[i].yi;
		particles[i].yi -= gravity;

		// if particle below the ground make it a new one
		if(particles[i].y <= -1) {
			initparticle(i);
		}
	}

	glDisable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
}

void initparticle(int i) {
	// set random x y based on gensize
	// we don't need seed anymore so just do good ol rand
	particles[i].x = rand() % gensize - gensize/2;
	particles[i].z = rand() % gensize - gensize/2;
	// 10 is about the sky
	particles[i].y = 20 + rand() % 10;
	// set it's lifetime
	particles[i].life = 1.0f;
	// set the color
	particles[i].r = snow[0];
	particles[i].g = snow[1];
	particles[i].b = snow[2];
	// make the falling rate a little random
	particles[i].yi = (rand() % 25) / -50.0;
}

void moveparticles(float x, float z) {
	for(int i = 0; i < MAX_PARTICLES; i++) {
		// I tried for a long time to try to get the particles
		// not to wobble when you move diagonally but I just 
		// can't figure it out
		particles[i].x -= x;
		particles[i].z -= z;
	}
}

void initparticles() {
	// initialize all our particles
	for(int i = 0; i < MAX_PARTICLES; i++) {
		initparticle(i);
	}
}