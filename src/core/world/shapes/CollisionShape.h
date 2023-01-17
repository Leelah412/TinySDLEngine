#ifndef __COLLISION_SHAPE_H__
#define __COLLISION_SHAPE_H__

#include <glm/glm.hpp>
#include <SDL.h>
#include <vector>

// TODO: replace SDL_FRect and remove any SDL dependency

// Base class for (2D) collision shapes to use for collision detection in PhysicsObject instances
class CollisionShape{
	
public:
	CollisionShape();
	virtual ~CollisionShape();

	virtual const std::vector<glm::vec2>& get_polygon() const;
	virtual void set_polygon(const std::vector<glm::vec2>& polygon);

	// IMPORTANT: There is NO set_aabb, since the bounding box is formed by the shape of the polygon
	virtual const SDL_FRect& get_aabb() const;

protected:
	// Outlier of the polygon marked through a sequence of points, with two neighboring points forming an edge each
	// First and last point form an edge, such that polygon becomes a closed path
	// Nodes are always relative to position of PhysicsObject
	std::vector<glm::vec2> m_polygon = {};
	// Axis-aligned minimum bounding box around the polygon
	SDL_FRect m_aabb = SDL_FRect();

};

#endif // !__COLLISION_SHAPE_H__
