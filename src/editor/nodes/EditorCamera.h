#ifndef __EDITOR_CAMERA_H__
#define __EDITOR_CAMERA_H__

#include <nodes/FirstPersonCamera.h>

namespace tse{

REGISTER_NODE(EditorCamera)

// EditorCamera to view a 3D Scene in the Editor
class EditorCamera : public FirstPersonCamera{

public:
	EditorCamera();
	// yes, the order is different compared to FPC
	EditorCamera(float viewport_width, float viewport_height, PROJECTION type = PROJECTION::PERSPECTIVE);
	virtual ~EditorCamera();

	// Override this to make sure camera only reacts to input, when we want it to
	virtual void handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext) override;

	bool is_focused();
	virtual void focus(bool _is_focused);

private:
	bool m_focused = true;			// Camera only moves, if mouse is focused on Scene viewport
};

}


#endif // !__EDITOR_CAMERA_H__
