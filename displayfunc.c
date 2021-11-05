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
// sun stuff ---------------------------- light stuff
//float Position[4];
double zh = 0;
int distance = 20;
int ylight = 0;
int local = 0;
// ----------------------------------
// player position
float playerpos[3] = { 0,0,0 };

void display() {
	// erase the window and depth buffer
	glClearColor(0.06, .92, .89, .5);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// enable z buffering
	glEnable(GL_DEPTH_TEST);
	// undo previous displays
	glLoadIdentity();

	// shade model ------------- light
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

	// draw ball representing light ------------ lighting stuff
	float Position[4] = { distance * Cos(zh), distance * Sin(zh), ylight, 1.0 };
	glColor3f(1, 1, 1);
	ball(Position[0], Position[1], Position[2], 5);
	// enable lighting stuff
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	// stuff in lighting but not in shaders
	//  Location of viewer for specular calculations
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local);
	//  glColor sets ambient and diffuse color materials
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	// back to in shaders
	glEnable(GL_LIGHT0);
	// set the lighting stuff
	float Ambient[] = {
		.1, .1, .1, 1.0
	};
	float Diffuse[] = {
		.5, .5, .5, 1.0
	};
	float Specular[] = {
		0, 0, 0, 1.0
	};
	glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
	glLightfv(GL_LIGHT0, GL_POSITION, Position);
	// lighting stuff ends ------------------------------------

	// draw our perlin ground
	glColor3f(.2, 1, .2);
	// set material stuff

	// disable cull face
	glDisable(GL_CULL_FACE);

	// lighting testing
	/*
	float white[] = { 1,1,1,1 };
	float black[] = { 0,0,0,1 };
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, .2);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-50, 0, -50);
	glVertex3f(-50, 0, 50);
	glVertex3f(50, 0, 50);
	glVertex3f(50, 0, -50);
	glEnd();
	*/

	//---------------- perlin ground
	int i, j;
	float white[] = { 1,1,1,1 };
	float black[] = { 0,0,0,1 };
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, .2);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
	for (i = 0; i < gensize-1; i++) {
		glBegin(GL_QUADS);
		for (j = 0; j < gensize-1; j++) {
			float y1 = perlin2d(i, j, .1, 4);
			float y2 = perlin2d(i, j + 1, .1, 4);
			float y3
			glVertex3f(i - 50, ground[i][j], j - 50);
			glVertex3f(i - 49, ground[i][j], j - 50);
			glVertex3f()
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

void initdebug() {
	dim = 50.0;
	proj = perspective;
}

// called when nothing else to do
void idle() {
	//  Elapsed time in seconds
	double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	zh = fmod(90 * t, 360.0);
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