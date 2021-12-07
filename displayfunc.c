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
int gensize = 200;
// sun stuff ---------------------------- light stuff
//float Position[4];
double zh = 180;
int distance = 100;
int ylight = 0;
int local = 0;
bool movelight = true;
float bg[4] = { 0.06, .92, .89, .5 };
// ----------------------------------
// player position
// random start so new world each time
float playerpos[3] = { 0,0,0 };
float playerangle = 0;
// perlin stuff
float pfreq = .2;
float pdep = 1;
// grass texture
unsigned int grasstex;

void display() {
	// erase the window and depth buffer
	// background color
	glClearColor(bg[0], bg[1], bg[2], bg[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// enable z buffering
	glEnable(GL_DEPTH_TEST);
	// undo previous displays
	glLoadIdentity();

	// make sure we normalize vectors
	glEnable(GL_NORMALIZE);

	// do the light stuff
	light();

	// for keeping track of ground points
	// out here so in for loop don't have to keep allocating memory
	float xpos;
	float zpos;
	// material for objects
	// setting one for all of them bc they all have roughly the same material in world gen
	float spec[] = { 1,6,1,1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1 / 20);
	
	// filling in the space square by square
	float i, j;
	for (i = -(gensize / 2); i < (gensize / 2) - 1; i++) {
		for (j = -(gensize / 2); j < (gensize / 2) - 1; j++) {
			// calculate the ground points
			xpos = playerpos[0] + i;
			zpos = playerpos[2] + j;
			// we draw around 0, 0 bc that's simplest but we sample perlin
			// as if we were moving around which gives the effect that we are moving
			// 2 perlin noises on top of each other look much better than 1
			float pos1[3] = {i, perlin2d(xpos, zpos, pfreq, pdep) + perlin2d(xpos, zpos, pfreq/5, pdep*4), j };
			float pos2[3] = {i + 1, perlin2d(xpos + 1, zpos, pfreq, pdep) + perlin2d(xpos + 1, zpos, pfreq/5, pdep*4), j };
			float pos3[3] = {i + 1, perlin2d(xpos + 1, zpos + 1, pfreq, pdep) + perlin2d(xpos + 1, zpos + 1, pfreq/5, pdep*4), j + 1 };
			float pos4[3] = {i, perlin2d(xpos, zpos + 1, pfreq, pdep) + perlin2d(xpos, zpos + 1, pfreq/5, pdep*4), j + 1 };
			// color for the ground
			glColor3f(.2, 1, .2);
			// draw the ground
			glBegin(GL_QUADS);
			// calculate the normal for the ground quads
			doanormal(pos1, pos2, pos3);
			// grass texture
			glBindTexture(GL_TEXTURE_2D, grasstex);
			// um, texture math is a little weird
			// I do it this way because I want the texture to go over the entire area
			// at the size it was made. So the ground is gensize x gensize and I want 
			// the texture to go across that once but also gotta move the texture as you move
			// so can't just say 0,0 1,1 at the two edges but gotta make it weird so it repeats
			// does that make any sense?
			// draw the ground
			int tx = (int)xpos;
			int ty = (int)zpos;
			//glTexCoord2f((tx % gensize) / gensize * 1.0, (ty % gensize) / gensize * 1.0);
			//glTexCoord2f(i, j);
			glVertex3f(pos1[0], pos1[1], pos1[2]);
			//glTexCoord2f(((tx + 1) % gensize) / gensize * 1.0, (ty % gensize) / gensize * 1.0);
			//glTexCoord2f(i+1, j);
			glVertex3f(pos2[0], pos2[1], pos2[2]);
			//glTexCoord2f(((tx + 1) % gensize) / gensize * 1.0, ((ty + 1) % gensize) / gensize * 1.0);
			//glTexCoord2f(i+1, j+1);
			glVertex3f(pos3[0], pos3[1], pos3[2]);
			//glTexCoord2f((tx % gensize) / gensize * 1.0, ((ty + 1) % gensize) / gensize * 1.0);
			//glTexCoord2f(i, j + 1);
			glVertex3f(pos4[0], pos4[1], pos4[2]);
			glEnd();
			// now it's time to populate this ground with some objects
			// first set our location for consistent generation based on position
			nLehmer = (int)zpos << 16 | (int)xpos;
			// draw a tree based on what lehmer got
			if (Lehmer32() % 256 < 2) {
				tree(i, pos1[1], j);
			}
		}
	}

	ErrCheck("display");
	glFlush();
	glutSwapBuffers();
}

// do the lighting stuff
void light() {
	// shade model ------------- light stuff -------------
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
		playerpos[1] = perlin2d(playerpos[0], playerpos[2], pfreq, pdep) + perlin2d(playerpos[0], playerpos[2], pfreq / 5, pdep * 4) + 1.5;
		gluLookAt(0, playerpos[1], 0,
			Cos(playerangle), playerpos[1], Sin(playerangle),
			0, 1, 0);
		break;
	}
	
	// enable lighting stuff
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	// stuff in lighting but not in shaders
	//  Location of viewer for specular calculations
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local);
	//  glColor sets ambient and diffuse color materials
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	// from 270 to 90 light increasing in brightness
	// from 90 to 270 light decreasing in brightness
	// for now just switch it on and off
	if (zh > 180) {
		bg[0] = 0;
		bg[1] = 0;
		bg[2] = 0;
		glDisable(GL_LIGHT0);
		glEnable(GL_LIGHT1);

		//  Translate intensity to color vectors
		float ambient = 0;
		float diffuse = 1;
		float specular = 0;
		float Ambient[] = { 0.01 * ambient ,0.01 * ambient ,0.01 * ambient ,1.0 };
		float Diffuse[] = { 0.01 * diffuse ,0.01 * diffuse ,0.01 * diffuse ,1.0 };
		float Specular[] = { 0.01 * specular,0.01 * specular,0.01 * specular,1.0 };
		float Position[] = { 0, playerpos[1], 0, 1 };
		//  Spotlight color and direction
		float yellow[] = { 1.0,1.0,0.0,1.0 };
		float Direction[] = { Cos(playerangle), 0, Sin(playerangle), 0 };
		//  Draw light position as ball (still no lighting here)
		ball(0, playerpos[1], 0, 0.1);
		//  OpenGL should normalize normal vectors
		glEnable(GL_NORMALIZE);
		//  Enable lighting
		glEnable(GL_LIGHTING);
		//  glColor sets ambient and diffuse color materials
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
		//  Set specular colors
		glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
		glMaterialf(GL_FRONT, GL_SHININESS, .2);
		//  Enable light 0
		glEnable(GL_LIGHT0);
		//  Set ambient, diffuse, specular components and position of light 0
		glLightfv(GL_LIGHT1, GL_AMBIENT, Ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, Diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, Specular);
		glLightfv(GL_LIGHT1, GL_POSITION, Position);
		//  Set spotlight parameters
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, Direction);
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20);
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, .2);
		//  Set attenuation
		glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.01);
		glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0);
		glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0);

		// fog bc fog cool
		glEnable(GL_FOG);
		glFogf(GL_FOG_MODE, GL_EXP);
		glFogfv(GL_FOG_COLOR, bg);
		glFogf(GL_FOG_DENSITY, .1);
		
	}
	else {
		// background needs to reflect changes in time of day
		bg[0] = 0.06;
		bg[1] = .92;
		bg[2] = .89;

		// draw ball representing light ------------ lighting stuff
		float Position[4] = { distance * Cos(zh), distance * Sin(zh), ylight, 1.0 };
		glColor3f(1, .2, .2);
		ball(Position[0], Position[1], Position[2], 5);
		
		glDisable(GL_LIGHT1);
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

		// fog bc fog cool
		glEnable(GL_FOG);
		glFogf(GL_FOG_MODE, GL_EXP);
		glFogfv(GL_FOG_COLOR, bg);
		glFogf(GL_FOG_DENSITY, .02);
	}
	
	// ------------------------------------------------------------- light stuff end
}

// originally for initializing debug stuff buuutttt
// it's set up nicely so I'm just going to use it
// for intializing everything
void initdebug() {
	// random start location
	// random lehmer seed
	nLehmer = rand() % 256;
	// really big random possibility space
	playerpos[0] = Lehmer32() % 5000;
	playerpos[2] = Lehmer32() % 5000;
	// load our grass texture the only texture we draw in this file
	glEnable(GL_TEXTURE_2D);
	// I can't decide if I want modular or not so I'mma just leave this here
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	grasstex = LoadTexBMP("textures/grass.bmp");
}

// called when nothing else to do
void idle() {
	if (movelight) {
		//  Elapsed time in seconds
		double t = glutGet(GLUT_ELAPSED_TIME) / 10000.0;
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
	// lighting testing, lets you move the light manually
	case 'z':
		zh += 30;
		break;
	case 'Z':
		zh -= 30;
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