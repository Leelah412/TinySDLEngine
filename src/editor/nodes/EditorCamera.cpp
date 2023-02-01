#include "EditorCamera.h"
#include <util/random_string.h>

namespace tse{
	EditorCamera::EditorCamera(){}
	EditorCamera::EditorCamera(float viewport_width, float viewport_height, PROJECTION type)
	: FirstPersonCamera(type, viewport_width, viewport_height){

	set_unique_name("editor_camera_" + get_ID() + random_string(32));
}

EditorCamera::~EditorCamera(){
	set_unique_name("editor_camera_" + get_ID() + random_string(32));
}

void EditorCamera::handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext){
	if(!m_focused) return;
	FirstPersonCamera::handle_event(sdl_event, event, ext);
}

bool EditorCamera::is_focused(){
	return m_focused;
}

void EditorCamera::focus(bool _is_focused){
	m_focused = _is_focused;
}

}