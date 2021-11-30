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

/*
 *  Compute natural cubic spline
 *  X is the array of points
 *  S is the second derivative (calculated here)
 *  k is the component
 *  n is the number of points
 */
void CubicSpline(double X[50][3], double S[50][3], int k, int j)
{
	double W[50];
	S[0][k] = W[0] = 0;
	for (int i = 1; i < j - 1; i++)
	{
		double f = 0.5 * S[i - 1][k] + 2;
		S[i][k] = -0.5 / f;
		W[i] = (3 * (X[i + 1][k] - 2 * X[i][k] + X[i - 1][k]) - 0.5 * W[i - 1]) / f;
	}
	S[n - 1][k] = 0;
	for (int i = j - 2; i > 0; i--)
		S[i][k] = S[i][k] * S[i + 1][k] + W[i];
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
	/*
	Color(.5, .6, .3);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex3f(x, y, z);
	glVertex3f(x, y+height, z);
	glEnd();
	*/

	// recursive functions that will populate our other branches
	branch(1, 5);
	branch(1, 5);

	// all the tree points were constructed so now draw a path between them
	//n -= 1;
	/*
	glColor3f(.4, .6, .3);
	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	int i = n;
	while (parents[i] != i) {
		glVertex3f(controls[i].x, controls[i].y, controls[i].z);
		i = parents[i];
	}
	glVertex3f(controls[i].x, controls[i].y, controls[i].z);
	glEnd();
	*/
	/*
	glColor3f(0, 0, 0);
	glPointSize(5);
	glBegin(GL_POINTS);
	int i = n;
	for (i = 0; i < n; i++)
		glVertex3f(controls[i].x, controls[i].y, controls[i].z);
	glEnd();
	*/

	glEnable(GL_MAP1_VERTEX_3);

	n -= 1;
	int i = n;
	struct point P[50];
	struct point S[50];
	int j = 0;
	while (parents[i] != i) {
		P[j] = controls[i];
		j += 1;
		i = parents[i];
	}
	P[j] = controls[i];

	/*
	// calculate (x,y,z) splines
	CubicSpline((void*)P, (void*)S, 0, j);
	CubicSpline((void*)P, (void*)S, 1, j);
	CubicSpline((void*)P, (void*)S, 2, j);
	// draw entire curve
	glColor3f(1, 1, 0);
	glBegin(GL_LINE_STRIP);
	int m = 25;
	for (int k = 0; k < j - 1; k++) {
		// cardinal point
		glVertex3f(P[k].x, P[k].y, P[k].z);
		// increment between k and k+1
		for (int f = 1; f < m; f++) {
			double f1 = (double)i / m;
			double g = 1 - f1;
			double f2 = (f1 * f1 * f1 - f1) / 6;
			double g2 = (g * g * g - g) / 6;
			double x = f1 * P[k + 1].x + g * P[k].x + f2 * S[k + 1].x + g2 * S[k].x;
			double y = f1 * P[k + 1].y + g * P[k].y + f2 * S[k + 1].y + g2 * S[k].y;
			double z = f1 * P[k + 1].z + g * P[k].z + f2 * S[k + 1].z + g2 * S[k].z;
			glVertex3f(x, y, z);
		}
	}
	// last cardinal point
	glVertex3f(P[j - 1].x, P[j - 1].y, P[j - 1].z);
	glEnd();
	*/
	//  Calculate (x,y,z) splines
	CubicSpline((void*)P, (void*)S, 0, n);
	CubicSpline((void*)P, (void*)S, 1, n);
	CubicSpline((void*)P, (void*)S, 2, n);
	//  Draw entire curve
	glColor3f(1, 1, 0);
	glBegin(GL_LINE_STRIP);
	int m = 25;
	for (int k = 0; k < n - 1; k++)
	{
		//  Cardinal point
		glVertex2d(P[k].x, P[k].y);
		//  Increment between k and k+1
		for (int i = 1; i < m; i++)
		{
			double f = (double)i / m;
			double g = 1 - f;
			double f2 = (f * f * f - f) / 6;
			double g2 = (g * g * g - g) / 6;
			double x = f * P[k + 1].x + g * P[k].x + f2 * S[k + 1].x + g2 * S[k].x;
			double y = f * P[k + 1].y + g * P[k].y + f2 * S[k + 1].y + g2 * S[k].y;
			double z = f * P[k + 1].z + g * P[k].z + f2 * S[k + 1].z + g2 * S[k].z;
			glVertex3d(x, y, z);
		}
	}
	//  Last cardinal point
	glVertex2d(P[n - 1].x, P[n - 1].y);
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

	// draw a line between the top and bottom
	/*
	Color(.5, .6, .3);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex3f(base.x, base.y, base.z);
	glVertex3f(tmppoint.x, tmppoint.y, tmppoint.z);
	glEnd();
	*/

	// call the next recursive stuff
	branch(parent+1, iterations);
	branch(parent+1, iterations);
}