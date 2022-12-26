#ifndef __COLLISION_SHAPE_H__
#define __COLLISION_SHAPE_H__

#include <SDL.h>
#include <vector>

// Base class for (2D) collision shapes to use for collision detection in PhysicsObject instances

using namespace std;

class CollisionShape{
	
public:
	CollisionShape();
	virtual ~CollisionShape();

	virtual const vector<SDL_FPoint>& get_polygon();
	virtual void set_polygon(vector<SDL_FPoint> polygon);

	// IMPORTANT: There is NO set_aabb, since the bounding box is formed by the shape of the polygon
	virtual SDL_FRect get_aabb();

protected:
	// Outlier of the polygon marked through a sequence of points, with two neighboring points forming an edge each
	// First and last point form an edge, such that polygon becomes a closed path
	// Nodes are always relative to position of PhysicsObject
	vector<SDL_FPoint> m_polygon = {};
	// Axis-aligned minimum bounding box around the polygon
	SDL_FRect m_aabb = SDL_FRect();

};

#endif // !__COLLISION_SHAPE_H__
