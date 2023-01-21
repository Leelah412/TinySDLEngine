#include "CollisionShape.h"

namespace tse{

CollisionShape::CollisionShape(){

}

CollisionShape::~CollisionShape(){

}

const std::vector<glm::vec2>& CollisionShape::get_polygon() const{
	return m_polygon;
}
void CollisionShape::set_polygon(const std::vector<glm::vec2>& polygon){
	m_polygon = polygon;
}

const SDL_FRect& CollisionShape::get_aabb() const{
	return m_aabb;
}

}
