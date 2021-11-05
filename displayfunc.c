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
// terrain stuff
int gensize = 100;
float ground[100][100];
// sun stuff
float Position = {
	
};

void display() {
	// erase the window and depth buffer
	glClearColor(0.06, .92, .89, .5);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// enable z buffering
	glEnable(GL_DEPTH_TEST);
	// undo previous displays
	glLoadIdentity();

	// shade model
	glShadeModel(GL_SMOOTH);

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

	// draw ball representing light
	glColor3f(1, 1, 1);
	ball(Position[0], Position[1], Position[2]);
	// enable lighting stuff
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	// stuff in lighting but not in shaders

	// back to in shaders
	glEnable(GL_LIGHT0);
	// set the lighting stuff
	glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
	glLightfv(GL_LIGHT0, GL_POSITION, Position);

	// draw our perlin ground
	glColor3f(.2, 1, .2);
	// set material stuff

	// disable cull face
	glDisable(GL_CULL_FACE);
	int i, j;
	for (i = 0; i < gensize-1; i++) {
		glBegin(GL_QUADS);
		for (j = 0; j < gensize; j++) {
			glVertex3f(i - 50, ground[i][j], j - 50);
			glVertex3f(i - 49, ground[i][j], j - 50);

		}
		glEnd();
	}

	// disable lighting
	glDisable(GL_LIGHTING);
	// draw some information to screen for debugging
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
	int i, j;
	for (i = 0; i < gensize; i++) {
		for (j = 0; j < gensize; j++) {
			ground[i][j] = perlin2d(i, j, .1, 4);
		}
	}
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