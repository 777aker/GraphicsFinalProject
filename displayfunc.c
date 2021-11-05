#include "displayfunc.h"

// aspect ratio
double asp = 1;
// size of world
double dim = 3.0;
// field of view
int fov = 55;
// projection type
enum camera proj = firstperson;
// aximuth of view angle
int th = 40;
// elevation of view angle
int ph = 15;

void display() {
	// erase the window and depth buffer
	glClearColor(0.06, .92, .89, .5);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// enable z buffering
	glEnable(GL_DEPTH_TEST);
	// undo previous displays
	glLoadIdentity();

	switch (proj) {
	case perspective:
		double Ex = -2 * dim * Sin(th) * Cos(ph);
		double Ey = +2 * dim * Sin(ph);
		double Ez = +2 * dim * Cos(th) * Cos(ph);
		gluLookAt(Ex, Ey, Ez, 0, 0, 0, 0, Cos(ph), 0);
		break;
	case orthogonal:
		glRotatef(ph, 1, 0, 0);
		glRotatef(th, 0, 1, 0);
		break;
	case firstperson:

		break;
	}

	glEnable(GL_CULL_FACE);
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	//glNormal3f(0, +1, 0);
	glColor3f(.2, 1, .2);
	glVertex3f(-50, 0, 50);
	glVertex3f(50, 0, 50);
	glVertex3f(50, 0, -50);
	glVertex3f(-50, 0, -50);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f(-50, 0, 50);
	glVertex3f(50, 0, 50);
	glVertex3f(50, 0, -50);
	glVertex3f(-50, 0, -50);
	glEnd();

	if (debug) {
		glColor3f(1, 1, 1);
		glWindowPos2i(5, 5);
		Print("th = %d, ph = %d, dim = %f", th, ph, dim);
	}

	ErrCheck("display");
	glFlush();
	glutSwapBuffers();
}

void initgrid() {

}

void initdebug() {
	dim = 50.0;
	proj = perspective;
}

// called when nothing else to do
void idle() {

	// redisplay the scene
	glutPostRedisplay();
}

// called when key is pressed
void key(unsigned char ch, int x, int y) {
	switch (ch) {
	// exit on ESC key
	case 27:
		exit(0);
	default:
		break;
	}

	// redisplay the scene
	Project(proj, fov, asp, dim);
	glutPostRedisplay();
}

// called when special key is pressed
void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_RIGHT:
		th += 5;
		break;
	case GLUT_KEY_LEFT:
		th -= 5;
		break;
	case GLUT_KEY_UP:
		ph += 5;
		break;
	case GLUT_KEY_DOWN:
		ph -= 5;
		break;
	case GLUT_KEY_PAGE_UP:
		dim += 1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		dim -= dim > 1 ? 1 : 0;
		break;
	default:
		break;
	}
	th %= 360;
	ph %= 360;

	// redisplay the scene
	Project(proj, fov, asp, dim);
	glutPostRedisplay();
}

// do this whenever window resized
void reshape(int width, int height) {
	// ratio of width to height of window
	asp = (height > 0) ? (double) width / height : 1;
	// set viewport to entire window
	glViewport(0, 0, RES * width, RES * height);
	// set projection
	Project(proj, fov, asp, dim);
}