#include "DynamicObject.h"

namespace tse{

DynamicObject::DynamicObject(){
	m_is_dynamic = true;
	set_unique_name("dynamic_object_" + to_string(get_ID()));
}

DynamicObject::~DynamicObject(){

}

void DynamicObject::update(const time_t& delta){
	move(delta);
}


void DynamicObject::move(const time_t& delta){
	glm::vec3 pos = get_position();
	// divide by 1000 to get velocity in pixel/second
	pos.x += m_velocity.x * (delta / 1000.0f);
	pos.y += m_velocity.y * (delta / 1000.0f);
	set_position(pos);
}

void DynamicObject::apply_force(const glm::vec3& force){
	apply_force(force.x, force.y, force.z);
}
void DynamicObject::apply_force(float x, float y, float z){
	m_velocity.x += x / m_mass;
	m_velocity.y += y / m_mass;
}

const glm::vec3& DynamicObject::get_velocity() const{
	return m_velocity;
}
void DynamicObject::set_velocity(const glm::vec3& velocity){
	m_velocity = velocity;
}
void DynamicObject::set_velocity(float x, float y, float z){
	set_velocity(glm::vec3(x, y, z));
}
const glm::vec3& DynamicObject::get_velocity_normalized() const{
	float norm = sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
	glm::vec3 p;
	p.x = m_velocity.x / norm;
	p.y = m_velocity.y / norm;
	return p;
}
float DynamicObject::get_speed() const{
	return sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
}

const glm::vec3& DynamicObject::get_center_of_mass() const{
	return m_center_of_mass;
}
void DynamicObject::set_center_of_mass(const glm::vec3& center_of_mass){
	m_center_of_mass = center_of_mass;
}
void DynamicObject::set_center_of_mass(float x, float y, float z){
	glm::vec3 p;
	p.x = x;
	p.y = y;
	set_center_of_mass(p);
}

bool DynamicObject::move_and_collide(DynamicObject* obj1, PhysicsObject* obj2, time_t delta, time_t& collision_time){

	// Set collision time to negative to indicate collision not found (currently)
	collision_time = -1;

	if(!obj1 || !obj2) return false;

	// Collision at current time
	if(delta == 0){
		cout << "COLLISION at current time (delta 0)!" << endl;
		bool _colliding = colliding(obj1, obj2);
		if(_colliding) collision_time = 0;
		return _colliding;
	}

	// Can't have collision, if there is no collision shape
	const CollisionShape* obj1_cs = obj1->get_collision_shape(), * obj2_cs = obj2->get_collision_shape();
	if(!obj1_cs || !obj2_cs) return false;
	SDL_FRect obj1_aabb = obj1_cs->get_aabb(), obj2_aabb = obj2_cs->get_aabb();
	glm::vec3 obj1_pos = obj1->get_global_position(), obj2_pos = obj2->get_global_position();

	// Get the entry and exit distances between both objects for both axes
	float x_entry = (obj2_pos.x + obj2_aabb.x) - (obj1_pos.x + obj1_aabb.x + obj1_aabb.w);
	float x_exit = (obj2_pos.x + obj2_aabb.x + obj2_aabb.w) - (obj1_pos.x + obj1_aabb.x);
	float y_entry = (obj2_pos.y + obj2_aabb.y) - (obj1_pos.y + obj1_aabb.y + obj1_aabb.h);
	float y_exit = (obj2_pos.y + obj2_aabb.y + obj2_aabb.h) - (obj1_pos.y + obj1_aabb.y);

	// Objects overlap on an axis, if signs of entry and exit are opposites
	// Objects collide, if both axis overlap
	if((((x_entry <= 0) && (x_exit >= 0)) || ((x_entry >= 0) && (x_exit <= 0))) &&
		(((y_entry <= 0) && (y_exit >= 0)) || ((y_entry >= 0) && (y_exit <= 0))))
		return true;

	// Get x and y speeds of both objects
	float obj1_vel_x = obj1->get_velocity().x;
	float obj1_vel_y = obj1->get_velocity().y;
	float obj2_vel_x = 0;
	float obj2_vel_y = 0;

	// Get the velocity of the second object, if it's dynamic (i.e. moving), otherwise it will be treated as static.
	DynamicObject* _obj2;
	if(_obj2 = dynamic_cast<DynamicObject*>(obj2)){
		obj2_vel_x = _obj2->get_velocity().x;
		obj2_vel_y = _obj2->get_velocity().y;
	}

	// Get the relative velocities (relative to first object)
	float vel_x = obj1_vel_x - obj2_vel_x;
	float vel_y = obj1_vel_y - obj2_vel_y;

	// If both relative velocities are zero, there can be no collision
	if((vel_x == 0.0f) && (vel_y == 0.0f)) return false;

	// Switch entry and exit, if moving in negative direction
	if(vel_x < 0.0f){
		float tmp = x_entry;
		x_entry = x_exit;
		x_exit = tmp;
	}
	if(vel_y < 0.0f){
		float tmp = y_entry;
		y_entry = y_exit;
		y_exit = tmp;
	}

	// If 1st object is right/bottom (i.e. entry distance < 0) and the velocity is positive,
	// or if 1st object is left/top (i.e. entry distance > 0) and the velocity is negative,
	// but the exit distance is has the opposite sign of the entry distance,
	// then the objects are already colliding in the corresponding axis.
	// 
	// However, if the sign of the entry and exit distance are the opposite of the relative velocity for a given axis,
	// then there was no collision, and there never will be, since the objects are moving further away from each other.
	if(((x_entry < 0) && (x_exit < 0) && (vel_x > 0)) || ((x_entry > 0) && (x_exit > 0) && (vel_x < 0))) return false;
	if(((y_entry < 0) && (y_exit < 0) && (vel_y > 0)) || ((y_entry > 0) && (y_exit > 0) && (vel_y < 0))) return false;

	// Calculate time, until entry/exit distances are 0'd with the velocitys of both objects, i.e.:
	// 
	//			entry - velocity * t = 0		<=>		t = entry / velocity
	// 
	//	entry in pixel, velocity in pixel/sec, t in sec

	// Negative time for the entry point means the collision entry has happened in the past already.
	// Meanwhile, a positive exit point time means the objects will stop colliding at that time.
	// If the exit point time is negative, it means the objects never collided and with the current velocity,
	// they won't be colliding anytime in the future either, though we've already dealt with that situation beforehand.
	float x_entry_t, x_exit_t, y_entry_t, y_exit_t;

	// Avoid division by 0
	if(vel_x == 0.0f){
		x_entry_t = -numeric_limits<float>::infinity();
		x_exit_t = numeric_limits<float>::infinity();
	}
	else{
		x_entry_t = x_entry / vel_x;
		x_exit_t = x_exit / vel_x;
	}
	if(vel_y == 0.0f){
		y_entry_t = -numeric_limits<float>::infinity();
		y_exit_t = numeric_limits<float>::infinity();
	}
	else{
		y_entry_t = y_entry / vel_y;
		y_exit_t = y_exit / vel_y;
	}

	// With this we've now got the overlap intervals, in which both objects collide on either axis.
	// To find the moment of collision, we have to find the moment, where the collision intervals of both axis overlap for the first time for t >= 0.
	// If the overlap doesn't exist, there is no collision.
	// If the overlap exists, but only for t < 0, there is no collision.

	if(x_exit_t < 0) return false;
	if(y_exit_t < 0) return false;

	// Collision occurs, if latest entry time on either axis occurs earlier than the earlist exit time on either axis.
	// Latest entry time would also be equivalent to the time of collision.
	collision_time = max(int(x_entry_t * 1000), int(y_entry_t * 1000));
	time_t exit_time = min(int(x_exit_t * 1000), int(y_exit_t * 1000));

	// If this condition ever becomes true, then there must be some kind of bug in the code.
	if(exit_time < collision_time){
		cerr << "ERROR: Somehow we've left the object before the collision?!" << endl;
		return false;
	}

	// Since we want to know, if collision occured during the given delta time, the collision time must not be bigger than delta
	if(collision_time > delta) return false;

#if DEBUG
	cout << "COLLISION!" << endl;
	cout << "collision_time: " << collision_time << endl;
	cout << "vel_x: " << vel_x << "; vel_y: " << vel_y << endl;
	cout << "obj1_vel_x: " << obj1_vel_x << "; obj1_vel_y: " << obj1_vel_y << endl;
	cout << "obj2_vel_x: " << obj2_vel_x << "; obj2_vel_y: " << obj2_vel_y << endl;
#endif

	return true;
}

}


