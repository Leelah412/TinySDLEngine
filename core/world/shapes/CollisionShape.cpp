#include "CollisionShape.h"

CollisionShape::CollisionShape(){

}

CollisionShape::~CollisionShape(){

}

const vector<SDL_FPoint>& CollisionShape::get_polygon(){
	return m_polygon;
}
void CollisionShape::set_polygon(vector<SDL_FPoint> polygon){
	m_polygon = polygon;
}

SDL_FRect CollisionShape::get_aabb(){
	return m_aabb;
}