#ifndef GLUTUTILITY_H
#define GLUTUTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#include "lehmer.h"

// GLEW _MUST_ be included first
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  Get all GL prototypes
#define GL_GLEXT_PROTOTYPES
//  Select SDL, SDL2, GLFW or GLUT
#if defined(SDL2)
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#elif defined(SDL)
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#elif defined(GLFW)
#include <GLFW/glfw3.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
//  Make sure GLU and GL are included
#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

// Cosine and Sine in degrees
// copying this from ex8 bc it's pretty useful
#define Cos(x) (cos((x)*3.14159265/180))
#define Sin(x) (sin((x)*3.14159265/180))

// --- variables ---
enum camera {
	perspective,
	orthogonal,
	firstperson,
	numtypes
};
extern bool debug;
// ok, if I have to make these one more time imma explode so
// defining them here as global variables
extern float black[4];
extern float white[4];

// --- functions ---
// convenience functions
void doanormal(float one[3], float two[3], float three[3]);
void Color(float r, float g, float b);
void Vertexflat(float thet, float height, float distance);
// convenience functions from class
void Print(const char* format , ...);
void Fatal(const char* format , ...);
unsigned int LoadTexBMP(const char* file);
void Project(enum camera cp, double fov, double asp, double dim);
void ErrCheck(const char* where);
int  LoadOBJ(const char* file);
void ball(double x, double y, double z, double r);
void Vertex(double th, double ph);

#endif