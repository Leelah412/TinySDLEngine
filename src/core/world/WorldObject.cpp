#include "WorldObject.h"

WorldObject::WorldObject(){

}
WorldObject::~WorldObject(){

}

Vector2 WorldObject::get_position(){
	return m_position;
}
float WorldObject::get_position_x(){
	return m_position.x;
}
float WorldObject::get_position_y(){
	return m_position.y;
}
void WorldObject::set_position(const Vector2& position){
	set_position(position.x, position.y);
}
void WorldObject::set_position(const float& x, const float& y){
	Vector2 p;
	p.x = x;
	p.y = y;
	m_position = p;
}
void WorldObject::set_position_x(const float& x){
	set_position(x, m_position.y);
}
void WorldObject::set_position_y(const float& y){
	set_position(m_position.x, y);
}

float WorldObject::get_scale(){
	return m_scale;
}
void WorldObject::set_scale(const float& scale){
	m_scale = scale;
}

float WorldObject::get_rotation(){
	return m_rotation;
}
void WorldObject::set_rotation(const float& rotation){
	// make sure the rotations are in (-360, 360)
	m_rotation = rotation - (int(rotation / 360) * 360);
}
