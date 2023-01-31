#ifndef __EDITOR_VIEWPORT_WIDGET_H__
#define __EDITOR_VIEWPORT_WIDGET_H__

#include <renderer/Renderer.h>
#include "Widget.h"
#include "../nodes/EditorCamera.h"

USE_RENDERER

namespace tse{

// SceneWidget rendering a game scene in an ImGui window
class SceneWidget : public Widget{

public:
	SceneWidget(int x = -1, int y = -1, int w = -1, int h = -1, PROJECTION proj_type = PROJECTION::PERSPECTIVE);
	virtual ~SceneWidget();

	virtual void update(const time_t& delta) override;
	virtual void draw() override;

private:
	EditorCamera* m_editor_cam;
	Framebuffer* m_framebuffer;
	PROJECTION m_projection_type;
};

}


#endif // !__EDITOR_VIEWPORT_WIDGET_H__
