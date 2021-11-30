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
// leaf nodes
int leaves[50];
int nleaves = 0;

typedef struct { double x, y, z; } Point;

//  Interpolation matrix for Bezier
static double Mi[4][4] =
{
   { 1.00000000, 0.00000000, 0.00000000, 0.00000000},
   {-0.83333333, 3.00000000,-1.50000000, 0.33333333},
   { 0.33333333,-1.50000000, 3.00000000,-0.83333333},
   { 0.00000000, 0.00000000, 0.00000000, 1.00000000},
};

// This routine multiplies a 4 x 4 matrix with a vector of 4 points.
void Pmult(double M[4][4], Point v[4], Point r[4])
{
	for (int i = 0; i < 4; i++)
	{
		r[i].x = r[i].y = r[i].z = 0;
		for (int k = 0; k < 4; k++)
		{
			r[i].x += M[i][k] * v[k].x;
			r[i].y += M[i][k] * v[k].y;
			r[i].z += M[i][k] * v[k].z;
		}
	}
}

Point bezier(Point p0, Point p1, Point p2, Point p3, float t) {
	Point bez;
	bez.x = pow(1 - t, 3) * p0.x 
		+ 3 * pow(1 - t, 2) * t * p1.x 
		+ 3 * (1 - t) * pow(t, 2) * p2.x 
		+ pow(t, 3) * p3.x;
	bez.y = pow(1 - t, 3) * p0.y 
		+ 3 * pow(1 - t, 2) * t * p1.y 
		+ 3 * (1 - t) * pow(t, 2) * p2.y 
		+ pow(t, 3) * p3.y;
	bez.z = pow(1 - t, 3) * p0.z
		+ 3 * pow(1 - t, 2) * t * p1.z
		+ 3 * (1 - t) * pow(t, 2) * p2.z
		+ pow(t, 3) * p3.z;
	return bez;
}

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
	nleaves = 0;

	// recursive functions that will populate our other branches
	branch(1, 5);
	branch(1, 5);

	// making our bezier curve
	glEnable(GL_MAP1_VERTEX_3);
	//glEnable(GL_MAP1_NORMAL);

	/*
	for (int j = 0; j < nleaves; j++) {
		Point P[4];
		int pn = 0;
		for (int i = leaves[j]; i > 0; i = parents[i]) {
			//printf("%i\n", i);
			P[pn].x = controls[i].x;
			P[pn].y = controls[i].y;
			P[pn].z = controls[i].z;
			pn += 1;
		}
		P[pn].x = controls[0].x;
		P[pn].y = controls[0].y;
		P[pn].z = controls[0].z;
		pn += 1;

		int m = 10;
		Point r[4];
		//  Transform 4 data points to 4 control points
		Pmult(Mi, P, r);
		//  Draw curve
		glColor3f(1, 1, 0);
		glMap1d(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, (void*)r);
		glMapGrid1f(m, 0.0, 1.0);
		glEvalMesh1(GL_LINE, 0, m);
	}
	*/

	for (int j = 0; j < nleaves; j++) {
		Point P[4];
		int pn = 0;
		for (int i = leaves[j]; i > 0; i = parents[i]) {
			P[pn].x = controls[i].x;
			P[pn].y = controls[i].y;
			P[pn].z = controls[i].z;
			pn += 1;
		}

		int m = 10;
		glColor3f(1, 1, 0);
		glPointSize(3);
		//printf("------------------------------\n");
		//printf("%f,%f,%f\n", P[0].x, P[0].y, P[0].z);
		//printf("%f,%f,%f\n", P[1].x, P[1].y, P[1].z);
		//printf("%f,%f,%f\n", P[2].x, P[2].y, P[2].z);
		//printf("%f,%f,%f\n", P[3].x, P[3].y, P[3].z);
		//printf("+++++++++++++++++++++++++\n");
		glBegin(GL_LINE_STRIP);
		for (int k = 0; k <= m; k++) {
			//printf("%f", (float)(k) / (float)(m));
			Point bezpt = bezier(P[0], P[1], P[2], P[3], (float)(k) / (float)(m));
			//printf("%f,%f,%f\n", bezpt.x, bezpt.y, bezpt.z);
			glVertex3f(bezpt.x, bezpt.y, bezpt.z);
		}
		glVertex3f(controls[0].x, controls[0].y, controls[0].z);
		glEnd();
	}

	// we are done put it back
	glPopMatrix();
}

void branch(int parent, int iterations) {
	// decrease iterations by 1
	iterations -= 1;
		
	// add some random stuff to our base
	struct point tmppoint = controls[parent];
	float negx = Lehmer32() % 2 == 0 ? 1.0 : -1.0;
	float negz = Lehmer32() % 2 == 0 ? 1.0 : -1.0;
	tmppoint.x += negx;
	tmppoint.y += (1 + ((float)(Lehmer32() % 10)-4)/10) * ratio * (1 + iterations/10);
	tmppoint.z += negz;
	
	controls[n] = tmppoint;
	parents[n] = parent;
	int thisparent = n;
	n += 1;

	if (iterations - 1 <= 1) {
		leaves[nleaves] = n-1;
		nleaves += 1;
		return;
	}

	// call the next recursive stuff
	branch(thisparent, iterations);
	branch(thisparent, iterations);
}