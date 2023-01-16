#include "CameraNode.h"

namespace tse{


CameraNode::CameraNode(){
	set_camera(new Camera());
}

CameraNode::CameraNode(Camera* camera){
	set_camera(camera);
}
CameraNode::CameraNode(PROJECTION type, float viewport_width, float viewport_height){
	set_camera(new Camera(type, viewport_width, viewport_height));
}

CameraNode::~CameraNode(){
	delete m_camera;
}

void CameraNode::update_global_position(){
	Node::update_global_position();
	if(!m_camera) return;
	m_camera->set_position(get_global_position());
}

Camera* CameraNode::get_camera() const{
	return m_camera;
}

void CameraNode::set_camera(Camera* camera){
	delete m_camera;
	m_camera = camera;
}

void CameraNode::activate_camera(){
	IRenderManager->activate_camera(m_camera);
}

void CameraNode::deactivate_camera(){
	IRenderManager->deactivate_camera(m_camera);
}


}