#ifndef __COLLISION_BOX_H__
#define __COLLISION_BOX_H__

#include "CollisionShape.h"

class CollisionBox : public CollisionShape{

public:
	CollisionBox();
	CollisionBox(SDL_FRect hitbox);
	CollisionBox(float x, float y, float w, float h);
	virtual ~CollisionBox();

	// Returns the collision box
	SDL_FRect get_hitbox();
	void set_hitbox(SDL_FRect rect);
	void set_hitbox(float x, float y, float w, float h);

	// Important: override the way paths are created, such that only "boxy" paths are accepted!
	virtual void set_polygon(vector<SDL_FPoint> polygon) override;


protected:
	SDL_FRect m_hitbox;
};

#endif // !__COLLISION_BOX_H__
