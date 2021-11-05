#include "displayfunc.h"

// aspect ratio
double asp = 1;
// size of world
double dim = 3.0;
// field of view
int fov = 55;
// projection type
int proj = 1;

void display() {
	// erase the window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// enable z buffering
	glEnable(GL_DEPTH_TEST);

	ErrCheck("display");
	glFlush();
	glutSwapBuffers();
}

// called when nothing else to do
void idle() {

	// redisplay the scene
	glutPostRedisplay();
}

// called when key is pressed
void key(unsigned char ch, int x, int y) {

	// redisplay the scene
	glutPostRedisplay();
}

// called when special key is pressed
void special(int key, int x, int y) {

	// redisplay the scene
	glutPostRedisplay();
}

// do this whenever window resized
void reshape(int width, int height) {
	// ratio of width to height of window
	asp = (height > 0) ? (double) width / height : 1;
	// set viewport to entire window
	glViewport(0, 0, RES * width, RES * height);
	// set projection
	Project(proj ? fov : 0, asp, dim);
}