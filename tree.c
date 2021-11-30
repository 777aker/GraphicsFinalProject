#include "tree.h"

// some global variables
// the trees width and height
float height;
float width;
float ratio;

// global tree structure that is all the control points
// for our bezier curve
struct point controls[50];
// the parent of each point
int parents[50];
// the first empty position
int n = 0;

// draw a tree
// I could've just copied from my previous hw, but 
// regretably I didn't and spent way too much time on this
void tree(int x, int y, int z) {
	// reset out matrix
	glPushMatrix();
	// Lehmer was set by display before calling this so don't need to change seed
	// make a random height and width for trees
	height = Lehmer32() % 8 + 3;
	width = (Lehmer32() % 3 + 1) * .1;
	// the ratio is actually used in generation
	// so multiplied width by 10 to make it not high values
	// so it is useful in generation
	ratio = height / (width * 10);

	// populate our first two values which are the trunks
	struct point bottom = { x,y,z };
	struct point top = { x,y + height,z };
	controls[0] = bottom;
	parents[0] = 0;
	controls[1] = top;
	parents[1] = 0;
	n = 2;

	// recursive functions that will populate our other branches
	branch(1, 5);
	branch(1, 5);

	// set n to how many points we have
	n -= 1;
	
	// draw the centers of our tree
	glBegin(GL_LINES);
	for (int i = n; i >= 0; i--) {
		glVertex3f(controls[i].x, controls[i].y, controls[i].z);
		glVertex3f(controls[parents[i]].x, controls[parents[i]].y, controls[parents[i]].z);
	}
	glEnd();

	// we are done put it back
	glPopMatrix();
}

void branch(int parent, int iterations) {
	// decrease iterations by 1
	iterations -= 1;
	if (iterations <= 1)
		return;
	// add some random stuff to our base
	struct point tmppoint = controls[parent];
	float negx = Lehmer32() % 2 == 0 ? 1.0 : -1.0;
	float negz = Lehmer32() % 2 == 0 ? 1.0 : -1.0;
	tmppoint.x += negx;
	tmppoint.y += (1 + ((float)(Lehmer32() % 10)-4)/10) * ratio * (1 + iterations/10);
	tmppoint.z += negz;
	
	controls[n] = tmppoint;
	parents[n] = parent;
	n += 1;

	// call the next recursive stuff
	branch(parent+1, iterations);
	branch(parent+1, iterations);
}