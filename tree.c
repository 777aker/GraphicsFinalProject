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

// nice Point structure from class
typedef struct { double x, y, z; } Point;
// x axis Point for computing stuff
Point x1 = { 1,0,0 };

// custom bezier function
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

// derivative of bezier
Point bezierdt(Point p0, Point p1, Point p2, Point p3, float t) {
	Point bezdt;
	bezdt.x = 3 * pow(1 - t, 2) * (p1.x - p0.x)
		+ 6 * (1 - t) * t * (p2.x - p1.x)
		+ 3 * pow(t, 2) * (p3.x - p2.x);
	bezdt.y = 3 * pow(1 - t, 2) * (p1.y - p0.y)
		+ 6 * (1 - t) * t * (p2.y - p1.y)
		+ 3 * pow(t, 2) * (p3.y - p2.y);
	bezdt.z = 3 * pow(1 - t, 2) * (p1.z - p0.z)
		+ 6 * (1 - t) * t * (p2.z - p1.z)
		+ 3 * pow(t, 2) * (p3.z - p2.z);
	return bezdt;
}

// cross product of two vectors
Point cross(Point p0, Point p1) {
	Point cp;
	cp.x = p0.y * p1.z - p0.z * p1.y;
	cp.y = p0.z * p1.x - p0.x * p1.z;
	cp.z = p0.x * p1.y - p0.y * p1.x;
	// normalize it
	float magnitude = pow((pow(cp.x, 2) + pow(cp.y, 2) + pow(cp.z, 2)), .5);
	cp.x /= magnitude;
	cp.y /= magnitude;
	cp.z /= magnitude;
	return cp;
}

// negative vector
Point negative(Point p0) {
	Point neg;
	neg.x = -1 * p0.x;
	neg.y = -1 * p0.y;
	neg.z = -1 * p0.z;
	return neg;
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
	/*
	glTranslatef(x, y, z);
	int i;
	glBegin(GL_QUADS);
	for (i = 360; i >= 0; i -= 60) {
		// normal
		glNormal3f(Sin(i - 30), 0, Cos(i - 30));
		// points
		Vertexflat(i, 0, width);
		Vertexflat(i, height, width);
		Vertexflat(i - 60, height, width);
		Vertexflat(i - 60, 0, width);
	}
	glEnd();
	glPopMatrix();
	*/

	// recursive functions that will populate our other branches
	branch(1, 5);
	branch(1, 5);

	// now build our many bezier curves
	for (int j = 0; j < nleaves; j++) {
		// first make our control points
		Point P[4];
		int pn = 0;
		for (int i = leaves[j]; i > 0; i = parents[i]) {
			P[pn].x = controls[i].x;
			P[pn].y = controls[i].y;
			P[pn].z = controls[i].z;
			pn += 1;
		}
		P[pn].x = controls[0].x;
		P[pn].y = controls[0].y;
		P[pn].z = controls[0].z;
		pn += 1;
		// store the bezier points
		Point bezpts[11];
		// evaluate our control points
		int m = 10;
		glColor3f(1, 1, 0);
		//glPointSize(3);
		//glBegin(GL_LINE_STRIP);
		Point offset1[11];
		Point offset2[11];
		Point offset3[11];
		Point offset4[11];
		for (int k = 0; k <= m; k++) {
			bezpts[k] = bezier(P[0], P[1], P[2], P[3], (float)(k) / (float)(m));
			Point v1 = bezierdt(P[0], P[1], P[2], P[3], (float)(k) / (float)(m));
			offset1[k] = cross(x1, v1);
			offset2[k] = cross(offset1[k], v1);
			offset3[k] = negative(offset1[k]);
			offset4[k] = negative(offset2[k]);
		}
		glColor3f(.8, .5, .4);
		glEnable(GL_CULL_FACE);
		glBegin(GL_QUADS);
		for (int k = 0; k < m; k++) {
			float tmpwidth = width * pow(k , .5) / 3;
			float one1[3] = { offset1[k].x,offset1[k].y,offset1[k].z };
			float two1[3] = { offset1[k+1].x,offset1[k+1].y,offset1[k+1].z };
			float three1[3] = { offset2[k + 1].x,offset2[k + 1].y,offset2[k + 1].z };
			doanormal(one1, two1, three1);

			glVertex3f(bezpts[k].x + offset1[k].x * tmpwidth, bezpts[k].y + offset1[k].y * tmpwidth, bezpts[k].z + offset1[k].z * tmpwidth);
			glVertex3f(bezpts[k + 1].x + offset1[k + 1].x * tmpwidth, bezpts[k + 1].y + offset1[k + 1].y * tmpwidth, bezpts[k + 1].z + offset1[k + 1].z * tmpwidth);
			glVertex3f(bezpts[k + 1].x + offset2[k + 1].x * tmpwidth, bezpts[k + 1].y + offset2[k + 1].y * tmpwidth, bezpts[k + 1].z + offset2[k + 1].z * tmpwidth);
			glVertex3f(bezpts[k].x + offset2[k].x * tmpwidth, bezpts[k].y + offset2[k].y * tmpwidth, bezpts[k].z + offset2[k].z * tmpwidth);

			float one2[3] = { offset2[k].x,offset2[k].y,offset2[k].z };
			float two2[3] = { offset2[k + 1].x,offset2[k + 1].y,offset2[k + 1].z };
			float three2[3] = { offset3[k + 1].x,offset3[k + 1].y,offset3[k + 1].z };
			doanormal(one2, two2, three2);

			glVertex3f(bezpts[k].x + offset2[k].x * tmpwidth, bezpts[k].y + offset2[k].y * tmpwidth, bezpts[k].z + offset2[k].z * tmpwidth);
			glVertex3f(bezpts[k + 1].x + offset2[k + 1].x * tmpwidth, bezpts[k + 1].y + offset2[k + 1].y * tmpwidth, bezpts[k + 1].z + offset2[k + 1].z * tmpwidth);
			glVertex3f(bezpts[k + 1].x + offset3[k + 1].x * tmpwidth, bezpts[k + 1].y + offset3[k + 1].y * tmpwidth, bezpts[k + 1].z + offset3[k + 1].z * tmpwidth);
			glVertex3f(bezpts[k].x + offset3[k].x * tmpwidth, bezpts[k].y + offset3[k].y * tmpwidth, bezpts[k].z + offset3[k].z * tmpwidth);

			float one3[3] = { offset3[k].x,offset3[k].y,offset3[k].z };
			float two3[3] = { offset3[k + 1].x,offset3[k + 1].y,offset3[k + 1].z };
			float three3[3] = { offset4[k + 1].x,offset4[k + 1].y,offset4[k + 1].z };
			doanormal(one3, two3, three3);

			glVertex3f(bezpts[k].x + offset3[k].x * tmpwidth, bezpts[k].y + offset3[k].y * tmpwidth, bezpts[k].z + offset3[k].z * tmpwidth);
			glVertex3f(bezpts[k + 1].x + offset3[k + 1].x * tmpwidth, bezpts[k + 1].y + offset3[k + 1].y * tmpwidth, bezpts[k + 1].z + offset3[k + 1].z * tmpwidth);
			glVertex3f(bezpts[k + 1].x + offset4[k + 1].x * tmpwidth, bezpts[k + 1].y + offset4[k + 1].y * tmpwidth, bezpts[k + 1].z + offset4[k + 1].z * tmpwidth);
			glVertex3f(bezpts[k].x + offset4[k].x * tmpwidth, bezpts[k].y + offset4[k].y * tmpwidth, bezpts[k].z + offset4[k].z * tmpwidth);

			float one4[3] = { offset4[k].x,offset4[k].y,offset4[k].z };
			float two4[3] = { offset4[k + 1].x,offset4[k + 1].y,offset4[k + 1].z };
			float three4[3] = { offset1[k + 1].x,offset1[k + 1].y,offset1[k + 1].z };
			doanormal(one4, two4, three4);

			glVertex3f(bezpts[k].x + offset4[k].x * tmpwidth, bezpts[k].y + offset4[k].y * tmpwidth, bezpts[k].z + offset4[k].z * tmpwidth);
			glVertex3f(bezpts[k + 1].x + offset4[k + 1].x * tmpwidth, bezpts[k + 1].y + offset4[k + 1].y * tmpwidth, bezpts[k + 1].z + offset4[k + 1].z * tmpwidth);
			glVertex3f(bezpts[k + 1].x + offset1[k + 1].x * tmpwidth, bezpts[k + 1].y + offset1[k + 1].y * tmpwidth, bezpts[k + 1].z + offset1[k + 1].z * tmpwidth);
			glVertex3f(bezpts[k].x + offset1[k].x * tmpwidth, bezpts[k].y + offset1[k].y * tmpwidth, bezpts[k].z + offset1[k].z * tmpwidth);
			
		}
		glEnd();
		glDisable(GL_CULL_FACE);
		
		/*
		glBegin(GL_LINE_STRIP);
		for (int k = 0; k <= m; k++) {
			glVertex3f(bezpts[k].x, bezpts[k].y, bezpts[k].z);
		}
		glEnd();
		for (int k = 0; k <= m; k++) {
			glBegin(GL_LINES);
			glVertex3f(bezpts[k].x, bezpts[k].y, bezpts[k].z);
			glVertex3f(bezpts[k].x + offset1[k].x * width, bezpts[k].y + offset1[k].y * width, bezpts[k].z + offset1[k].z * width);
			glVertex3f(bezpts[k].x, bezpts[k].y, bezpts[k].z);
			glVertex3f(bezpts[k].x + offset2[k].x * width, bezpts[k].y + offset2[k].y * width, bezpts[k].z + offset2[k].z * width);
			glEnd();
		}
		*/
		
		//glVertex3f(controls[0].x, controls[0].y, controls[0].z);
		//glEnd();
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

	if (iterations - 1 <= 2) {
		leaves[nleaves] = n-1;
		nleaves += 1;
		return;
	}

	// call the next recursive stuff
	branch(thisparent, iterations);
	branch(thisparent, iterations);
}