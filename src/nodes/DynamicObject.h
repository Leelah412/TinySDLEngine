#ifndef __MOVEABLE_OBJECT_H__
#define __MOVEABLE_OBJECT_H__

#include <nodes/PhysicsObject.h>

// TODO: make this inherit from physics upcoming physics interface

// Physics objects with the ability to move around in the game world

class DynamicObject : public PhysicsObject{

public:
	DynamicObject();
	virtual ~DynamicObject();

	virtual void update(const time_t& delta) override;

	// PHYSICS
	virtual void move(const time_t& delta);
	virtual void apply_force(const SDL_FPoint& force);				// apply a two dimensional force onto the object
	virtual void apply_force(const float& x, const float& y);

	virtual SDL_FPoint get_velocity();
	virtual void set_velocity(const SDL_FPoint& velocity);
	virtual void set_velocity(const float& x, const float& y);
	virtual SDL_FPoint get_velocity_normalized();					// return the normalized velocity vector
	virtual float get_speed();										// length of velocity vector

	virtual float get_mass();
	virtual void set_mass(const float& mass);

	virtual SDL_FPoint get_center_of_mass();
	virtual void set_center_of_mass(const SDL_FPoint& center_of_mass);
	virtual void set_center_of_mass(const float& x, const float& y);

	static bool move_and_collide(DynamicObject* obj1, PhysicsObject* obj2, time_t delta, time_t& collision_time);

private:
	SDL_FPoint m_velocity = SDL_FPoint();							// current velocity of the object (pixel/sec)
	float m_mass = 1;												// mass of the object, needed for applying force
	SDL_FPoint m_center_of_mass = SDL_FPoint();						// center of mass relative to center of object (i.e. position)

};

#endif