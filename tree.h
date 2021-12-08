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

// makes a tree
void tree(int x, int y, int z);
// draw the branches to a tree
// a recursive function that makes a few control points for tree branches
void branch(int parent, int iterations);
// the leaf generator that generates leaves at the end of a branch

// need some intialization stuff
void treeinit();

#endif // !TREE_H
