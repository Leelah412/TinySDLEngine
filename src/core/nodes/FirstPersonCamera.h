#ifndef __FIRST_PERSON_CAMERA_H__
#define __FIRST_PERSON_CAMERA_H__

#include "SceneLoader.h"
#include "CameraNode.h"

USE_EVENT_MANAGER

// TODO: delete
USE_RENDER_MANAGER

namespace tse{

REGISTER_NODE(FirstPersonCamera)

class FirstPersonCamera : public CameraNode{

public:
	FirstPersonCamera();
	FirstPersonCamera(Camera* camera);
	FirstPersonCamera(PROJECTION type, float viewport_width, float viewport_height);
	virtual ~FirstPersonCamera();

	// TODO: delete
	virtual void handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext) override;
	virtual void update(const time_t& delta) override;

private:
	bool m_left = false;
	bool m_right = false;
	bool m_up = false;
	bool m_down = false;
	bool m_in = false;
	bool m_out = false;

	float m_deg_per_px = 0.2;

	float m_angle_x = 0;
	float m_angle_y = 0;
	float m_mouse_x = 0;
	float m_mouse_y = 0;
};

}


#endif // !__FIRST_PERSON_CAMERA_H__
