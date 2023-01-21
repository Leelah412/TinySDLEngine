#ifndef __COLLISION_BOX_H__
#define __COLLISION_BOX_H__

#include "CollisionShape.h"

namespace tse{

class CollisionBox : public CollisionShape{

public:
	CollisionBox();
	CollisionBox(const SDL_FRect& hitbox);
	CollisionBox(float x, float y, float w, float h);
	virtual ~CollisionBox();

	// Returns the collision box
	const SDL_FRect& get_hitbox() const;
	void set_hitbox(const SDL_FRect& rect);
	void set_hitbox(float x, float y, float w, float h);

	// Important: override the way paths are created, such that only "boxy" paths are accepted!
	virtual void set_polygon(const std::vector<glm::vec2>& polygon) override;


protected:
	SDL_FRect m_hitbox;
};

}


#endif // !__COLLISION_BOX_H__
