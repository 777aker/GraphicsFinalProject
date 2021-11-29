#include "tree.h"
#include "glututility.h"

// draw a tree
// taken from my hw6 but edited significantly to work in this context and look better
// and just generally do better practices and efficiency and performance
void tree(int x, int y, int z) {
	// reset out matrix
	glPushMatrix();
	// Lehmer was set by display before calling this so don't need to change seed
	// make a random height and width for trees
	float height = Lehmer32() % 8 + 3;
	float width = Lehmer32() % 3 *.1;

	// translate our tree to the position
	glTranslatef(x, y, z);
	// rotate the tree randomly for more random intrigue
	glRotatef(Lehmer32() % 360, 0, 1, 0);
	//glScalef(.25, .25, .25);
	// set the color to brown
	Color(79, 28, 3);

	// draw the trunk
	int i;
	glBegin(GL_QUADS);
	for (i = 360; i >= 0; i -= 60) {
		// calculate the normal for this face
		glNormal3f(Sin(i - 30), 0, Cos(i - 30));
		// draw the face
		Vertexflat(i, 0, width);
		Vertexflat(i, height, width);
		Vertexflat(i - 60, height, width);
		Vertexflat(i - 60, 0, width);
	}
	glEnd();

	// now some branches


	// we are done put it back
	glPopMatrix();
}

