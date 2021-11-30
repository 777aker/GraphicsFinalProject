#include "tree.h"

// some global variables
// the trees width and height
float height;
float width;
float ratio;

// void tree type
struct trees voidtype = {
	{ -1, -20, -1},
	{ -1, -20, -1}
};

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
	ratio = height / (width * 10);

	/* pre bezier
	// translate our tree to the position
	glTranslatef(x, y, z);
	// rotate the tree randomly for more random intrigue
	glRotatef(Lehmer32() % 360, 0, 1, 0);
	// set the color to brown
	Color(79, 28, 3);
	struct point base[6];

	// draw the trunk
	int i;
	glBegin(GL_QUADS);
	// glVertex3f(Sin(thet)*distance, height, Cos(thet)*distance);
	for (i = 360; i >= 0; i -= 60) {
		// calculate the normal for this face
		glNormal3f(Sin(i - 30), 0, Cos(i - 30));
		// draw the face
		glVertex3f(Sin(i) * width, 0, Cos(i) * width);
		// we need to store our bases for the branches
		struct point tmpbase = { Sin(i) * width, height, Cos(i) * width };
		base[i / 60 - 1] = tmpbase;
		glVertex3f(Sin(i) * width, height, Cos(i) * width);
		glVertex3f(Sin(i - 60) * width, height, Cos(i - 60) * width);
		glVertex3f(Sin(i - 60) * width, 0, Cos(i - 60) * width);
	}
	glEnd();
	

	// now some branches
	struct point tmpbase[] = { base[0],base[1],base[2],base[3] };
	branch(tmpbase);
	struct point tmpbase2[] = { base[2],base[3],base[4],base[5] };
	branch(tmpbase2);
	*/
	struct trees tmptree;
	struct point tmppoint = { x, y, z };
	tmptree.base = tmppoint;
	tmppoint.y += height;
	tmptree.top = tmppoint;

	Color(.5, .6, .3);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex3f(tmptree.base.x, tmptree.base.y, tmptree.base.z);
	glVertex3f(tmptree.top.x, tmptree.top.y, tmptree.top.z);
	glEnd();

	branch(tmptree, 5);
	branch(tmptree, 5);
	

	// we are done put it back
	glPopMatrix();
}

void branch(struct trees base, int iterations) {
	// find the midpoint of the base
	iterations -= 1;
	if (iterations <= 1)
		return;
	struct trees tmptree;
	tmptree.base = base.top;
	struct point tmppoint = base.top;
	float negx = Lehmer32() % 2 == 0 ? 1.0 : -1.0;
	float negz = Lehmer32() % 2 == 0 ? 1.0 : -1.0;
	//tmppoint.x += negx * (float)(Lehmer32() % iterations)*ratio*(float)(pow(iterations, .5));
	tmppoint.x += negx;
	tmppoint.y += (1 + ((float)(Lehmer32() % 10)-4)/10) * ratio * (1 + iterations/10);
	//tmppoint.z += negz * (float)(Lehmer32() % iterations)*ratio*(float)(pow(iterations, .5));
	tmppoint.z += negz;
	tmptree.top = tmppoint;

	Color(.5, .6, .3);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex3f(tmptree.base.x, tmptree.base.y, tmptree.base.z);
	glVertex3f(tmptree.top.x, tmptree.top.y, tmptree.top.z);
	glEnd();

	branch(tmptree, iterations);
	branch(tmptree, iterations);
}