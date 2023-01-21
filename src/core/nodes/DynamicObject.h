#ifndef __MOVEABLE_OBJECT_H__
#define __MOVEABLE_OBJECT_H__

#include "SceneLoader.h"
#include <nodes/PhysicsObject.h>

// TODO: make this inherit from physics upcoming physics interface

// Physics objects with the ability to move around in the game world


namespace tse{

REGISTER_NODE(DynamicObject)

class DynamicObject : public PhysicsObject{

public:
	DynamicObject();
	virtual ~DynamicObject();

	virtual void update(const time_t& delta) override;

	// PHYSICS
	virtual void move(const time_t& delta);
	virtual void apply_force(const glm::vec3& force);					// apply a two dimensional force onto the object
	virtual void apply_force(float x, float y, float z);

	virtual const glm::vec3& get_velocity() const;
	virtual void set_velocity(const glm::vec3& velocity);
	virtual void set_velocity(float x, float y, float z);
	virtual const glm::vec3& get_velocity_normalized() const;			// return the normalized velocity vector
	virtual float get_speed() const;									// length of velocity vector

	virtual const glm::vec3& get_center_of_mass() const;
	virtual void set_center_of_mass(const glm::vec3& center_of_mass);
	virtual void set_center_of_mass(float x, float y, float z);

	static bool move_and_collide(DynamicObject* obj1, PhysicsObject* obj2, time_t delta, time_t& collision_time);

private:
	glm::vec3 m_velocity = glm::vec3();								// current velocity of the object (pixel/sec)
	glm::vec3 m_center_of_mass = glm::vec3();						// center of mass relative to center of object (i.e. position)

};

}



#endif