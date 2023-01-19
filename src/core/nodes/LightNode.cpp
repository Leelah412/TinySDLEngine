#include "LightNode.h"
#include <glm/gtx/string_cast.hpp>

namespace tse{

LightNode::LightNode(){

}
LightNode::LightNode(Light* light){
	set_light(light);
}
LightNode::LightNode(const Light& light){
	set_light(light);
}
LightNode::~LightNode(){

}

void LightNode::set_light(Light* light){
	delete m_light;
	m_light = light;
}

void LightNode::set_light(const Light& light){
	delete m_light;
	set_light(new Light(light));
}

void LightNode::update_global_position(){
	Node::update_global_position();
	if(!m_light) return;
	m_light->position = glm::vec4(get_global_position(), 1.0f);
}

void LightNode::update_global_rotation(){
	Node::update_global_rotation();
	if(!m_light) return;
	glm::vec3 rot = get_global_rotation();
	glm::vec3 dir;
	dir.x = cos(glm::radians(-rot.y)) * cos(glm::radians(rot.x));
	dir.y = sin(glm::radians(-rot.x));
	dir.z = sin(glm::radians(-rot.y)) * cos(glm::radians(rot.x));
	std::cout << glm::to_string(dir) << std::endl;
	m_light->direction = glm::vec4(glm::normalize(dir), 0.0f);
}

void LightNode::turn_on(){
	IRenderManager->activate_light(m_light);
}

void LightNode::turn_off(){
	IRenderManager->deactivate_light(m_light);
}

}