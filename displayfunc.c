#include "displayfunc.h"

// aspect ratio
double asp = 1;
// size of world
double dim = 20.0;
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
int distance = 120;
int ylight = 0;
int local = 0;
bool movelight = true;
// ----------------------------------
// player position
float playerpos[3] = { 0,0,0 };
float playerangle = 0;
// perlin stuff
float pfreq = .2;
float pdep = 1;

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
	default:
		proj = perspective;
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
		playerpos[1] = perlin2d(playerpos[0], playerpos[2], pfreq, pdep) + 1.5;
		gluLookAt(0, playerpos[1], 0,
			Cos(playerangle), playerpos[1], Sin(playerangle),
			0, 1, 0);
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
	// -------------------------------------------------------------

	float xpos;
	float zpos;

	// green color for ground
	glColor3f(.2, 1, .2);
	// material for objects
	float white[] = { 1,1,1,1 };
	float black[] = { 0,0,0,1 };
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, .2);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);

	// filling in the space square by square
	int i, j;
	for (i = -(gensize / 2); i < (gensize / 2) - 1; i++) {
		for (j = -(gensize / 2); j < (gensize / 2) - 1; j++) {
			// calculate the ground points
			xpos = playerpos[0] + i;
			zpos = playerpos[2] + j;
			// we draw around 0, 0 bc that's simplest but we sample perlin
			// as if we were moving around which gives the effect that we are moving
			float pos1[3] = {i, perlin2d(xpos, zpos, pfreq, pdep), j };
			float pos2[3] = {i + 1, perlin2d(xpos + 1, zpos, pfreq, pdep), j };
			float pos3[3] = {i + 1, perlin2d(xpos + 1, zpos + 1, pfreq, pdep), j + 1 };
			float pos4[3] = {i, perlin2d(xpos, zpos + 1, pfreq, pdep), j + 1 };
			// draw the ground
			glBegin(GL_QUADS);
			// calculate the normal for the ground quads
			doanormal(pos1, pos2, pos3);
			// draw the ground
			glVertex3f(pos1[0], pos1[1], pos1[2]);
			glVertex3f(pos2[0], pos2[1], pos2[2]);
			glVertex3f(pos3[0], pos3[1], pos3[2]);
			glVertex3f(pos4[0], pos4[1], pos4[2]);
			glEnd();
		}
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
	//dim = 50.0;
	//proj = perspective;
}

// called when nothing else to do
void idle() {
	if (movelight) {
		//  Elapsed time in seconds
		double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
		zh = fmod(90 * t, 360.0);
	}
	// redisplay the scene
	glutPostRedisplay();
}

// called when key is pressed
void key(unsigned char ch, int x, int y) {
	switch (ch) {
	// exit on ESC key
	case 27:
		exit(0);
		break;
	// movement stuff
	case 'w':
		playerpos[0] += Cos(playerangle);
		playerpos[2] += Sin(playerangle);
		break;
	case 's':
		playerpos[0] -= Cos(playerangle);
		playerpos[2] -= Sin(playerangle);
		break;
	case 'a':
		playerpos[0] -= Cos(playerangle + 90);
		playerpos[2] -= Sin(playerangle + 90);
		break;
	case 'd':
		playerpos[0] += Cos(playerangle + 90);
		playerpos[2] += Sin(playerangle + 90);
		break;
	case 'e':
		playerangle += 2;
		break;
	case 'q':
		playerangle -= 2;
		break;
	// projection type
	case 'p':
		proj += 1 % numtypes;
		break;
	// whether or not to move the light
	case '1':
		movelight = !movelight;
		break;
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
	// move perspective and orthogonal camera around
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