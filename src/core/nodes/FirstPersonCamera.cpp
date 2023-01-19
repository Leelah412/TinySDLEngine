#include "FirstPersonCamera.h"


namespace tse{



FirstPersonCamera::FirstPersonCamera()
	: CameraNode(){

	EventManager->add_event_listener(SEM_EVENT::SEM_KEY_DOWN, this);
	EventManager->add_event_listener(SEM_EVENT::SEM_KEY_UP, this);
	EventManager->add_event_listener(SEM_EVENT::SEM_MOUSE_MOVE, this);
}

FirstPersonCamera::FirstPersonCamera(Camera* camera)
	: CameraNode(camera){
	EventManager->add_event_listener(SEM_EVENT::SEM_KEY_DOWN, this);
	EventManager->add_event_listener(SEM_EVENT::SEM_KEY_UP, this);
	EventManager->add_event_listener(SEM_EVENT::SEM_MOUSE_MOVE, this);
}

FirstPersonCamera::FirstPersonCamera(PROJECTION type, float viewport_width, float viewport_height)
	: CameraNode(type, viewport_width, viewport_height){

	EventManager->add_event_listener(SEM_EVENT::SEM_KEY_DOWN, this);
	EventManager->add_event_listener(SEM_EVENT::SEM_KEY_UP, this);
	EventManager->add_event_listener(SEM_EVENT::SEM_MOUSE_MOVE, this);
}

FirstPersonCamera::~FirstPersonCamera(){

	EventManager->remove_event_listener(SEM_EVENT::SEM_KEY_DOWN, this, false);
	EventManager->remove_event_listener(SEM_EVENT::SEM_KEY_UP, this, false);
	EventManager->remove_event_listener(SEM_EVENT::SEM_MOUSE_MOVE, this, false);
}

void FirstPersonCamera::handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext){
	if(sdl_event.type == SDL_KEYDOWN){
		switch(sdl_event.key.keysym.sym){
			case SDLK_a: m_left = true; break;
			case SDLK_LEFT: m_left = true; break;
			case SDLK_d: m_right = true; break;
			case SDLK_RIGHT: m_right = true; break;
			case SDLK_w: m_up = true; break;
			case SDLK_UP: m_up = true; break;
			case SDLK_s: m_down = true; break;
			case SDLK_DOWN: m_down = true; break;
			case SDLK_q: m_in = true; break;
			case SDLK_e: m_out = true; break;
			default: break;
		}
	}
	else if(sdl_event.type == SDL_KEYUP){
		switch(sdl_event.key.keysym.sym){
			case SDLK_a: m_left = false; break;
			case SDLK_LEFT: m_left = false; break;
			case SDLK_d: m_right = false; break;
			case SDLK_RIGHT: m_right = false; break;
			case SDLK_w: m_up = false; break;
			case SDLK_UP: m_up = false; break;
			case SDLK_s: m_down = false; break;
			case SDLK_DOWN: m_down = false; break;
			case SDLK_q: m_in = false; break;
			case SDLK_e: m_out = false; break;
			default: break;
		}
	}
	else if(event == SEM_MOUSE_MOVE){
		int x = sdl_event.motion.xrel, y = sdl_event.motion.yrel;
		m_angle_x += (float)(-x) * m_deg_per_px;
		m_angle_y += (float)(y)*m_deg_per_px;
		m_mouse_x += x;
		m_mouse_y += y;
	}
}



void FirstPersonCamera::update(const time_t& delta){

	const float speed = 2.0f;

	float sec = (float)((double)delta / 1000.0);
	float x_factor = sec * (m_left - m_right) * speed;
	float z_factor = sec * (m_down - m_up) * speed;
	float y_factor = sec * (m_out - m_in) * speed;

	// ROTATION

	// calc new relative rotation by adding difference between new and old rotation to curretn relative rotation
	// we do NOT want to set the rotation of the camera with its own functions, instead use "set_rotation", such that node and all its childrens rotations get updated
	glm::vec3 l_rot = glm::vec3(get_rotation().x + m_angle_y, get_rotation().y + m_angle_x, 0);
	set_rotation(l_rot);
	m_angle_x = 0;
	m_angle_y = 0;

	if(m_camera->get_pitch() > 89.0f)
		m_camera->set_pitch(89.0f);
	if(m_camera->get_pitch() < -89.0f)
		m_camera->set_pitch(-89.0f);

	const float yaw = m_camera->get_yaw(), pitch = m_camera->get_pitch();

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), forwards_alt;

	// get the direction vector we are facing at
	glm::vec3 direction;
	direction.x = cos(glm::radians(-yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(-pitch));
	direction.z = sin(glm::radians(-yaw)) * cos(glm::radians(pitch));
	glm::vec3 front = glm::normalize(direction);
	forwards_alt = front;

	// move forward on the 2D plane instead of going directly towards the direction we are facing
	direction.x = cos(glm::radians(-yaw)) * cos(glm::radians(0.0f));
	direction.y = sin(glm::radians(0.0f));
	direction.z = sin(glm::radians(-yaw)) * cos(glm::radians(0.0f));
	glm::vec3 forwards = glm::normalize(direction);

	direction.x = cos(glm::radians(0.0f)) * cos(glm::radians(-pitch));
	direction.y = sin(glm::radians(-pitch));
	direction.z = sin(glm::radians(0.0f)) * cos(glm::radians(-pitch));


	// POSITION

	// multiply z factor by forwards vector, if we don't want to "lift up"
	// otherwise multiply by front vector
	glm::vec3 pos = get_global_position();
	//pos -= forwards * z_factor;							// move in direction of view (2D)
	pos -= forwards_alt * z_factor ;						// move in direction of view (3D)
	pos -= glm::normalize(glm::cross(front, up)) * x_factor;
	// pos is now, what new global position of node and cam should be
	// we want to add the difference between the new and the old global positions to the relative position
	pos = get_position() + pos - get_global_position();
	set_position(pos);
	glm::mat4 look_at = glm::lookAt(pos, pos + front, up);

	m_camera->set_view_matrix(look_at);
}



}