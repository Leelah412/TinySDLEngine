#include "LightNode.h"

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
	if(m_light->direction.w == 1.0f)
		m_light->direction = glm::vec4(get_global_position(), 1.0f);
}

void LightNode::turn_on(){
	IRenderManager->activate_light(m_light);
}

void LightNode::turn_off(){
	IRenderManager->deactivate_light(m_light);
}

}