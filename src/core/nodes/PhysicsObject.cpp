#include "PhysicsObject.h"


namespace tse{

PhysicsObject::PhysicsObject(){
	m_class = "PhysicsObject";
	m_is_dynamic = false;
	set_unique_name("physics_object_" + to_string(get_ID()));
	//cout << "node with unique name " << get_unique_name() << " initialized" << endl;
}

PhysicsObject::~PhysicsObject(){

}

void PhysicsObject::handle_collision(const PhysicsObject* colliding_with, time_t collision_time, const glm::vec3& collision_point){

}

void PhysicsObject::update_global_position(){
	// we do not want to do any changes to the way global dimensions are calculated,
	// we only want to know, WHEN they are changed, so just call function from Node
	Node::update_global_position();
	// TODO: also update collision tree
}
void PhysicsObject::update_global_rotation(){
	Node::update_global_rotation();
}
void PhysicsObject::update_global_scale(){
	Node::update_global_scale();
}

float PhysicsObject::get_mass() const{
	return m_mass;
}
void PhysicsObject::set_mass(float mass){
	m_mass = mass;
}

bool PhysicsObject::is_dynamic(){
	return m_is_dynamic;
}


const CollisionShape* PhysicsObject::get_collision_shape() const{
	return m_collision_shape;
}
void PhysicsObject::set_collision_shape(CollisionShape* collision_shape){
	m_collision_shape = collision_shape;
}
SDL_FRect PhysicsObject::get_global_aabb(){
	SDL_FRect rect;
	SDL_FRect aabb = get_collision_shape()->get_aabb();
	rect.x = get_global_position_x() + aabb.x;
	rect.y = get_global_position_y() + aabb.y;
	rect.w = get_global_position_x() + aabb.x + aabb.w;
	rect.h = get_global_position_y() + aabb.y + aabb.h;

	return rect;
}

bool PhysicsObject::colliding(const PhysicsObject* obj1, const PhysicsObject* obj2){
	if(!obj1 || !obj2) return false;
	// Can't have collision, if there is no collision shape
	const CollisionShape* obj1_cs = obj1->get_collision_shape(), * obj2_cs = obj2->get_collision_shape();
	if(!obj1_cs || !obj2_cs) return false;
	// Compare AABB first
	glm::vec3 obj1_pos = obj1->get_global_position(), obj2_pos = obj2->get_global_position();
	SDL_FRect obj1_aabb = obj1_cs->get_aabb(), obj2_aabb = obj2_cs->get_aabb();
	// right border of obj1 to the left of left border of obj2 -> not colliding
	if(obj1_pos.x + obj1_aabb.x + obj1_aabb.w < obj2_pos.x + obj2_aabb.x) return false;
	// left border of obj1 to the right of the right border of obj2 -> not colliding
	if(obj1_pos.x + obj1_aabb.x > obj2_pos.x + obj2_aabb.x + obj2_aabb.w) return false;
	// lower border of obj1 higher than upper border of obj2 -> not colliding
	if(obj1_pos.y + obj1_aabb.y + obj1_aabb.h < obj2_pos.y + obj2_aabb.y) return false;
	// higher border of obj1 lower than lower border of obj2 -> not colliding
	if(obj1_pos.y + obj1_aabb.y > obj2_pos.y + obj2_aabb.y + obj2_aabb.h) return false;

	//cout << "right of obj1: " << obj1_pos.x + obj1_aabb.x + obj1_aabb.w << "; left of obj2: " << obj2_pos.x + obj2_aabb.x << endl;

	// TODO: implement narrowed down collision detection with actual polygons
	// AABBs colliding, return true
	return true;
}

}