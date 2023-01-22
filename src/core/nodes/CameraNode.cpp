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

void CameraNode::update_global_rotation(){
	Node::update_global_rotation();
	if(!m_camera) return;
	glm::vec3 rot = get_global_rotation();
	m_camera->set_pitch(rot.x);
	m_camera->set_yaw(rot.y);
	m_camera->set_roll(rot.z);
}

Camera* CameraNode::get_camera() const{
	return m_camera;
}

void CameraNode::set_camera(Camera* camera){
	delete m_camera;
	m_camera = camera;
}

JSON CameraNode::save(){
	JSON data = Node::save();
	if(is_exempt_from_saving()) return data;

	if(!m_camera) return data;

	glm::mat4 vm = m_camera->get_view_matrix();
	JSON view = {
		{vm[0][0], vm[0][1], vm[0][2], vm[0][3]},
		{vm[1][0], vm[1][1], vm[1][2], vm[1][3]},
		{vm[2][0], vm[2][1], vm[2][2], vm[2][3]},
		{vm[3][0], vm[3][1], vm[3][2], vm[3][3]},
	};

	data["camera"] = {
		{"type", (int)m_camera->get_projection_type()},
		{"viewport_x", m_camera->get_viewport_x()},
		{"viewport_y", m_camera->get_viewport_y()},
		{"viewport_width", m_camera->get_viewport_width()},
		{"viewport_height", m_camera->get_viewport_height()},
		{"aspect_ratio", m_camera->get_aspect_ratio()},
		{"fov", m_camera->get_fov()},
		{"near_plane", m_camera->get_near_plane()},
		{"far_plane", m_camera->get_far_plane()},

		{"view_matrix", view}

	};
	
	return data;
}

void CameraNode::load(const JSON& data){
	Node::load(data);

	set_camera(new Camera());

	JSON camera;
	if(!data.contains("camera") || !(camera = data["camera"]).is_object()){
		std::cout << "WARNING: Loading 'CameraNode': 'camera' doesn't exist or is not an object!" << std::endl;
		return;
	}

	JSON it1;

	// Load the viewport stuff
	
	// PROJECTION
	if(camera.contains("type") && (it1 = camera["type"]).is_number_integer()){
		switch((int)it1){
			case PROJECTION::ORTHOGONAL:
				m_camera->set_projection_type(PROJECTION::ORTHOGONAL);
				break;
			case PROJECTION::PERSPECTIVE:
			default:
				m_camera->set_projection_type(PROJECTION::PERSPECTIVE);
				break;
		}
	}
	else{
		m_camera->set_projection_type(PROJECTION::PERSPECTIVE);
		std::cout << "WARNING: Loading 'CameraNode': 'camera.type' doesn't exist or is not an integer!" << std::endl;
	}

	// VIEWPORT 
	if(camera.contains("viewport_x") && (it1 = camera["viewport_x"]).is_number()){
		m_camera->set_viewport_x(it1);
	}
	else{
		m_camera->set_viewport_x(0);
		std::cout << "WARNING: Loading 'CameraNode': 'camera.viewport_x' doesn't exist or is not a number!" << std::endl;
	}

	if(camera.contains("viewport_y") && (it1 = camera["viewport_y"]).is_number()){
		m_camera->set_viewport_y(it1);
	}
	else{
		m_camera->set_viewport_y(0);
		std::cout << "WARNING: Loading 'CameraNode': 'camera.viewport_y' doesn't exist or is not a number!" << std::endl;
	}

	if(camera.contains("viewport_width") && (it1 = camera["viewport_width"]).is_number()){
		m_camera->set_viewport_width(it1);
	}
	else{
		m_camera->set_viewport_width(1280);
		std::cout << "WARNING: Loading 'CameraNode': 'camera.viewport_width' doesn't exist or is not a number!" << std::endl;
	}

	if(camera.contains("viewport_height") && (it1 = camera["viewport_height"]).is_number()){
		m_camera->set_viewport_height(it1);
	}
	else{
		m_camera->set_viewport_height(720);
		std::cout << "WARNING: Loading 'CameraNode': 'camera.viewport_height' doesn't exist or is not a number!" << std::endl;
	}

	// ASPECT RATIO 
	if(camera.contains("aspect_ratio") && (it1 = camera["aspect_ratio"]).is_number()){
		m_camera->set_aspect_ratio(it1);
	}
	else{
		// No need to set aspect ratio; automatically updated, when we set viewport width and height!
		std::cout << "WARNING: Loading 'CameraNode': 'camera.aspect_ratio' doesn't exist or is not a number!" << std::endl;
	}

	// FOV
	if(camera.contains("fov") && (it1 = camera["fov"]).is_number()){
		m_camera->set_fov(it1);
	}
	else{
		m_camera->set_fov(70.0f);
		std::cout << "WARNING: Loading 'CameraNode': 'camera.fov' doesn't exist or is not a number!" << std::endl;
	}

	// NEAR PLANE
	if(camera.contains("near_plane") && (it1 = camera["near_plane"]).is_number()){
		m_camera->set_near_plane(it1);
	}
	else{
		m_camera->set_near_plane(1.0f);
		std::cout << "WARNING: Loading 'CameraNode': 'camera.near_plane' doesn't exist or is not a number!" << std::endl;
	}

	// FAR PLANE
	if(camera.contains("far_plane") && (it1 = camera["far_plane"]).is_number()){
		m_camera->set_far_plane(it1);
	}
	else{
		m_camera->set_far_plane(1000.0f);
		std::cout << "WARNING: Loading 'CameraNode': 'camera.far_plane' doesn't exist or is not a number!" << std::endl;
	}

	// Load the nodes transformations into the camera
	m_camera->set_position(get_global_position());
	m_camera->set_pitch(get_global_pitch());
	m_camera->set_yaw(get_global_yaw());
	m_camera->set_roll(get_global_roll());

	// We want the projection matrix to be based on the view values
	// Must be called AFTER setting viewport stuff
	m_camera->update_projection_matrix();

	// View matrix must be loaded AFTER setting the projection matrix (currently),
	// since it's is also assigned a value, when updating the projection matrix
	glm::mat4 view = glm::mat4();

	// VIEW MATRIX
	// Since the view matrix is last and doesn't have to be assigned a default value at an error, we can just return at the first error
	if(!camera.contains("view_matrix") || !(it1 = camera["view_matrix"]).is_array() || (it1.size() != 4)){
		// No need to set view matrix here, as it is set, when updating the projection matrix already!
		std::cout << "WARNING: Loading 'CameraNode': 'camera.view_matrix' doesn't exist or is not a array or does not have the correct size (4)!" << std::endl;
		return;
	}
	for(int i = 0; i < 4; i++){
		if(!it1[i].is_array() || (it1[i].size() != 4)){
			std::cout << "WARNING: Loading 'CameraNode': 'camera.view_matrix' row is not an array or does not have the correct size (4)!" << std::endl;
			return;
		}
		for(int j = 0; j < 4; j++){
			if(!it1[i][j].is_number()){
				std::cout << "WARNING: Loading 'CameraNode': 'camera.view_matrix' must exclusively consist of numbers!" << std::endl;
				return;
			}
			view[i][j] = it1[i][j];
		}
	}

	m_camera->set_view_matrix(view);
}

void CameraNode::activate_camera(){
	IRenderManager->activate_camera(m_camera);
}

void CameraNode::deactivate_camera(){
	IRenderManager->deactivate_camera(m_camera);
}


}