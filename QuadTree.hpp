///=============================================================================
///
/// \file           QuadTree.hpp
///
///
/// \author         Samy Duc, nobunaga.duc@gmail.com
///
/// \brief          Dynamic QuadTree
///
///=============================================================================

// Include guard
#ifndef QUADTREE_HPP_INCLUDED
#define QUADTREE_HPP_INCLUDED

#include "Point2d.hpp"
#include "Entity.h"
#include <list>
#include <cmath>

// T : objects store in leaf
class QuadTree
{
public:
	// Create quadtree given :
	// position : center position
	// size : width, height
	QuadTree(Point2d<float> position, Point2d<float> size, unsigned int depth, unsigned int maxDepth, unsigned int maxObjects, QuadTree* father);
	~QuadTree();

	bool insert(Entity*);
	bool erase(Entity*);
	// delete object
	// try only in this quadtree
	void eraseQuadObject(Entity *object);
	void getListCollision(Entity*, std::list<Entity*>*);

// Methods
protected:
	// give next quad following depth where Entity belongs
	QuadTree* getNextQuad(Entity*);
	// subdivise space in 4 and reposition objects
	void spacePartition();
	// detect if a rectangle can fit in the given quadtree
	bool quadFit(Entity *object);
	bool isObjectBelow();

// Properties
protected:
	Point2d<float> m_position;
	Point2d<float> m_size;
	unsigned int m_depth;
	unsigned int m_maxDepth;
	unsigned int m_maxObjects;
	QuadTree* m_father;
	// object below in the tree hierarchy
	bool m_objectBelow;

	QuadTree *upperLeft;
	QuadTree *upperRight;
	QuadTree *lowerLeft;
	QuadTree *lowerRight;

	std::list<Entity*> m_objectList;
};

#endif