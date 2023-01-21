#ifndef __CAMERA_NODE_H__
#define __CAMERA_NODE_H__

#include "SceneLoader.h"
#include "Node.h"
#include "renderer/Camera.h"

#include <string>

USE_RENDER_MANAGER

namespace tse{

REGISTER_NODE(CameraNode)

class CameraNode : public Node{

public:
	CameraNode();
	CameraNode(Camera* camera);
	CameraNode(PROJECTION type, float viewport_width, float viewport_height);
	virtual ~CameraNode();

	virtual void activate_camera();
	virtual void deactivate_camera();

	// Update camera position anytime global position is updated
	virtual void update_global_position() override;
	virtual void update_global_rotation() override;

	Camera* get_camera() const;
	virtual void set_camera(Camera* camera);

protected:
	Camera* m_camera = nullptr;

	short m_draw_index = -1;				// Position in the render sequence; negative, if inactive
	bool m_draw_on_window = true;			// Draw into the window framebuffer, if this is true, otherwise draw into an invisible one
	std::string m_framebuffer_name = "";	// Name of the framebuffer to draw onto; draws into default, if empty string or "m_draw_on_window" is true
	// m_viewport_rect						// Determines part of default framebuffer (i.e. visible window) to draw on
};


}


#endif // !__CAMERA_NODE_H__
