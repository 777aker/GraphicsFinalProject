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

// textures for trees
// just need trunk and leaves probably
unsigned int textures[2];

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

// multiply a vector
Point mult(Point p0, float scale) {
	Point cp;
	cp.x = p0.x * scale;
	cp.y = p0.y * scale;
	cp.z = p0.z * scale;
	return cp;
}

// do a normal off a point so I don't type so much
void glnormalpoint(Point p0) {
	glNormal3f(p0.x, p0.y, p0.z);
}

// also do a vertex off two points because that's a lot of typing too
void glvertex2points(Point p0, Point p1) {
	glVertex3f(p0.x + p1.x, p0.y + p1.y, p0.z + p1.z);
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
	parents[0] = -1; 
	controls[1] = top;
	parents[1] = 0;
	n = 2;
	nleaves = 0;

	// recursive functions that will populate our other branches
	branch(1, 5);
	branch(1, 5);

	// now build our many bezier curves
	for (int j = 0; j < nleaves; j++) {
		// first make our control points
		Point P[4];
		int pn = 0;
		// travel from a leaf up its parents till you reach the base to build the tree
		for (int i = leaves[j]; i >= 0; i = parents[i]) {
			P[pn].x = controls[i].x;
			P[pn].y = controls[i].y;
			P[pn].z = controls[i].z;
			pn += 1;
		}
		// store the bezier points
		Point bezpts[11];
		// evaluate our control points
		int m = 10;
		glColor3f(1, 1, 0);
		// also store the offset vectors for the points
		Point offset1[11];
		Point offset2[11];
		Point offset3[11];
		Point offset4[11];
		for (int k = 0; k <= m; k++) {
			// calculate our beziers
			bezpts[k] = bezier(P[0], P[1], P[2], P[3], (float)(k) / (float)(m));
			Point v1 = bezierdt(P[0], P[1], P[2], P[3], (float)(k) / (float)(m));
			// first cross product with x unit vector so we can get a normal
			// since it is unit vector x everytime we can simplify the cross product greatly
			Point cp;
			cp.x = 0;
			cp.y = v1.z;
			cp.z = v1.y;
			// normalize it
			float magnitude = pow(pow(cp.y, 2) + pow(cp.z, 2), .5);
			cp.y /= magnitude;
			cp.z /= magnitude;
			// build our four normals to the current point so we can make a thick tree
			offset1[k] = cp;
			offset2[k] = cross(cp, v1);
			offset3[k] = negative(cp);
			offset4[k] = negative(offset2[k]);
			// do our width here instead of being dumb and overly complicated later
			// varying width because want it to get thinner as you go up the tree
			float curwidth = width * pow(k + 1, .5) / 3;
			offset1[k] = mult(offset1[k], curwidth);
			offset2[k] = mult(offset2[k], curwidth);
			offset3[k] = mult(offset3[k], curwidth);
			offset4[k] = mult(offset4[k], curwidth);
		}

		// now we gonna draw some boys
		//glColor3f(.8, .5, .4);
		glColor3f(1, 1, 1);
		glEnable(GL_CULL_FACE);

		// textures please
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, textures[0]);

		glBegin(GL_QUADS);
		for (int k = 0; k < m; k++) {

			// fun fact, our normals are our offsets bc I'm cool like that
			// first quad
			float jk = (float) k / (float) (m);
			float jkp1 = (float)(k + 1) / (float) (m);
			float size = .5;
			//printf("%f,%f\n", jk, jkp1);
			glnormalpoint(offset1[k]);
			glTexCoord2f(0, jk);
			glvertex2points(bezpts[k], offset1[k]);

			glnormalpoint(offset1[k + 1]);
			glTexCoord2f(0, jkp1);
			glvertex2points(bezpts[k + 1], offset1[k + 1]);

			glnormalpoint(offset2[k + 1]);
			glTexCoord2f(size, jkp1);
			glvertex2points(bezpts[k + 1], offset2[k + 1]);

			glnormalpoint(offset2[k]);
			glTexCoord2f(size, jk);
			glvertex2points(bezpts[k], offset2[k]);

			// second quad
			glnormalpoint(offset2[k]);
			glTexCoord2f(size, jk);
			glvertex2points(bezpts[k], offset2[k]);

			glnormalpoint(offset2[k + 1]);
			glTexCoord2f(size, jkp1);
			glvertex2points(bezpts[k + 1], offset2[k + 1]);

			glnormalpoint(offset3[k + 1]);
			glTexCoord2f(2*size, jkp1);
			glvertex2points(bezpts[k + 1], offset3[k + 1]);

			glnormalpoint(offset3[k]);
			glTexCoord2f(2*size, jk);
			glvertex2points(bezpts[k], offset3[k]);

			// thrid quad
			glnormalpoint(offset3[k]);
			glTexCoord2f(2*size, jk);
			glvertex2points(bezpts[k], offset3[k]);
			
			glnormalpoint(offset3[k + 1]);
			glTexCoord2f(2*size, jkp1);
			glvertex2points(bezpts[k + 1], offset3[k + 1]);

			glnormalpoint(offset4[k + 1]);
			glTexCoord2f(3*size, jkp1);
			glvertex2points(bezpts[k + 1], offset4[k + 1]);

			glnormalpoint(offset4[k]);
			glTexCoord2f(3*size, jk);
			glvertex2points(bezpts[k], offset4[k]);

			// forth quad
			glnormalpoint(offset4[k]);
			glTexCoord2f(3*size, jk);
			glvertex2points(bezpts[k], offset4[k]);

			glnormalpoint(offset4[k + 1]);
			glTexCoord2f(3*size, jkp1);
			glvertex2points(bezpts[k + 1], offset4[k + 1]);

			// dang, should've done normal and points together in a function oh well
			glnormalpoint(offset1[k + 1]);
			glTexCoord2f(4*size, jkp1);
			glvertex2points(bezpts[k + 1], offset1[k + 1]);

			glnormalpoint(offset1[k]);
			glTexCoord2f(4*size, jk);
			glvertex2points(bezpts[k], offset1[k]);
		}
		glEnd();
		glDisable(GL_CULL_FACE);
		glDisable(GL_TEXTURE_2D);
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
	
	// add this new control point to controls and also
	// set who it's parent is
	controls[n] = tmppoint;
	parents[n] = parent;
	int thisparent = n;
	n += 1;

	// if this is our last iteration then put it in leaves array
	if (iterations - 1 <= 2) {
		leaves[nleaves] = n-1;
		nleaves += 1;
		return;
	}

	// call the next recursive stuff
	branch(thisparent, iterations);
	branch(thisparent, iterations);
}

void treeinit() {
	textures[0] = LoadTexBMP("textures/treeTrunks.bmp");
}