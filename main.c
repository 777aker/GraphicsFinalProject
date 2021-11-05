#include "main.h"

int main(int argc, char* argv[]) {

	// initialize GLUT
	glutInit(&argc, argv);
	glutInitWindowSize(600, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutCreateWindow("Graphics Final Project Kelley Kelley");
#ifdef USEGLEW
	//intialize GLEW
	if (glewInit() != GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
	// set callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);

	ErrCheck("init");

	glutMainLoop();
	
	return 0;
}