#ifndef __PHYSICS_OBJECT_H__
#define __PHYSICS_OBJECT_H__

#include "SceneLoader.h"
#include <nodes/Node.h>
#include <world/shapes/collision_shapes.h>


REGISTER_NODE(PhysicsObject)

class PhysicsObject : public Node{

public:

	PhysicsObject();
	virtual ~PhysicsObject();

	// Called, when a collision has occurred.
	// Assumption, that whoever is calling this function is telling the truth,
	// no check, whether collision has actually occurred or not.
	virtual void handle_collision(const PhysicsObject* colliding_with, time_t collision_time, const glm::vec3& collision_point);

	virtual const CollisionShape* get_collision_shape() const;
	virtual void set_collision_shape(CollisionShape* collision_shape);
	virtual SDL_FRect get_global_aabb();

	// Override global dimensional properties to apply changes in the collision tree
	virtual void update_global_position() override;
	virtual void update_global_rotation() override;
	virtual void update_global_scale() override;

	virtual float get_mass() const;
	virtual void set_mass(float mass);

	bool is_dynamic();

	static bool colliding(const PhysicsObject* obj1, const PhysicsObject* obj2);
	
protected:
	CollisionShape* m_collision_shape = nullptr;						// collision shape of the object
	float m_mass = 1;													// mass of the object

	bool m_is_dynamic;													// avoid casting as often as possible with this dynamic-object-check

};

#endif // !__PHYSICS_OBJECT_H__
