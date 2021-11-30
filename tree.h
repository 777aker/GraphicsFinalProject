#ifndef TREE_H
#define TREE_H

#include "lehmer.h"
#include "glututility.h"

// for passing around points
struct point {
	float x;
	float y;
	float z;
};
// constructing trees
struct trees {
	struct point base;
	struct point top;
};

// makes a tree
void tree(int x, int y, int z);
// draw the branches to a tree
// a recursive function that draws until they are too thin
void branch(struct trees base, int iterations);
// the leaf generator that generates leaves at the end of a branch


#endif // !TREE_H
