#ifndef __COLLISION_HANDLER_H__
#define __COLLISION_HANDLER_H__

#include "util/Object.h"
#include <nodes/Node.h>
#include <nodes/PhysicsObject.h>
#include <nodes/DynamicObject.h>
#include <world/shapes/collision_shapes.h>
#include <glm/glm.hpp>

#include <map>
#include <set>
#include <limits>

#define ICollisionHandler tse::CollisionHandler::get_default_collision_handler()
#define SetCollisionHandler(CH) tse::CollisionHandler::set_default_collision_handler(CH)

namespace tse{

// Active or possible collisions between dynamic and physics type
struct Collision{
	float impact_time;									// time of impact relative to current frame

	DynamicObject* dynamic_object;
	PhysicsObject* physics_object;						// can be dynamic or static

	glm::vec3 normal_dy;								// direction normal of dynamic object, when it hit the physics object
	glm::vec3 normal_ph;								// direction normal of physics object, when it hit the physics object (irrelevant, when static)

	glm::vec3 hitpoint_dy;								// point of impact on dynamic object relative to origin
	glm::vec3 hitpoint_ph;								// point of impact on physics object relative to origin

	// Collisions are sorted by time of impact
	bool operator<(const Collision& col) const{
		return impact_time < col.impact_time;
	}
	bool operator<=(const Collision& col) const{
		return impact_time <= col.impact_time;
	}
	bool operator==(const Collision& col) const{
		return impact_time == col.impact_time;
	}
	bool operator!=(const Collision& col) const{
		return impact_time != col.impact_time;
	}
	bool operator>=(const Collision& col) const{
		return impact_time >= col.impact_time;
	}
	bool operator>(const Collision& col) const{
		return impact_time > col.impact_time;
	}
};

// Collision handler should be called before assigning new positions for collision objects
class CollisionHandler : public Object{

public:
	CollisionHandler();
	~CollisionHandler();

	void update(time_t delta);

	void add_object(PhysicsObject* obj);
	void remove_object(PhysicsObject* obj);

	// Update list with current positions (without checking for intermediate collisions)
	void update_sweep_list();

	//multimap<float, PhysicsObject*> get_sweep_list();
	multiset<Collision> get_active_collisions();

	void update_on_signal(SIGNAL signal, EVENT* event) override;

	static CollisionHandler* get_default_collision_handler();
	static void set_default_collision_handler(CollisionHandler* handler);

private:
	void sweep(time_t delta);
	void narrow_detection(PhysicsObject* obj1, PhysicsObject* obj2);

	void sweep_helper_assign_velocity(time_t delta);		// Assign new positions to dynamic objects based on their velocity before starting sweeping
	void sweep_helper_sweep();								// Sweep


	set<PhysicsObject*> m_physics_objects;					// static physics objects with information on start/end point in sweep list
	set<DynamicObject*> m_dynamic_objects;					// objects with ability to move around with information on start/end point in sweep list
	map<float, set<PhysicsObject*>> m_sweep_static;			// sweep-and-prune list for static objects
	map<float, set<DynamicObject*>> m_sweep_dynamic;		// sweep-and-prune list for dynamic objects

	multiset<Collision> m_active_collisions;				// active collisions at current time (static and dynamic objects)

	static CollisionHandler* s_default_collision_handler;
};

}



// TODO: make collision nodes inherit interfaces below and completely decouple collision handler from node tree
/*

// Base class for all objects with the ability to physically interact with the game world
class ICollisionObject{

public:
	ICollisionObject();
	virtual ~ICollisionObject();

	static bool colliding(ICollisionObject* obj1, ICollisionObject* obj2);

	virtual CollisionShape* get_collision_shape();
	virtual void set_collision_shape(CollisionShape* collision_shape);

	virtual float get_mass();
	virtual void set_mass(const float& mass);

	virtual glm::vec3 get_center_of_mass();
	virtual void set_center_of_mass(const glm::vec3& center_of_mass);
	virtual void set_center_of_mass(const float& x, const float& y);

private:
	CollisionShape* m_collision_shape = nullptr;						// collision shape of the object

	float m_mass = 1;												// mass of the object, needed for applying force
	glm::vec3 m_center_of_mass = glm::vec3();						// center of mass relative to center of object (i.e. position)

};


class IDynamicObject : public ICollisionObject{

public:
	IDynamicObject();
	virtual ~IDynamicObject();

	// PHYSICS
	virtual void move(const time_t& delta);
	virtual void apply_force(const glm::vec3& force);				// apply a two dimensional force onto the object
	virtual void apply_force(const float& x, const float& y);

	virtual glm::vec3 get_velocity();
	virtual void set_velocity(const glm::vec3& velocity);
	virtual void set_velocity(const float& x, const float& y);
	virtual glm::vec3 get_velocity_normalized();					// return the normalized velocity vector
	virtual float get_speed();										// length of velocity vector

	static bool move_and_collide(IDynamicObject* obj1, ICollisionObject* obj2, float delta, float& collision_time);

private:
	glm::vec3 m_velocity = glm::vec3();							// current velocity of the object (pixel/sec)

};

*/


#endif // !__COLLISION_HANDLER_H__
