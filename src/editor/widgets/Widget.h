#ifndef __EDITOR_WIDGET_H__
#define __EDITOR_WIDGET_H__

#include <core/nodes/Node.h>
#include <util/Config.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <string>
#include <map>
#include <iostream>

namespace tse{

// Base class for Editor Widgets
// Uses Node class to make use of the Node tree
class Widget : public Node{

public:
	// Negative values means ImGui handles the sizing manually
	Widget(int x = -1, int y = -1, int w = -1, int h = -1);
	virtual ~Widget() = 0;

	void render();

	virtual void show_widget(bool show);
	bool is_visible();

protected:
	int m_x;				// X Position
	int m_y;				// Y Position
	int m_w;				// Width
	int m_h;				// Height

	bool m_show = true;

	// Render function for derived classes
	virtual void draw() override = 0;
};

}



#endif // !__EDITOR_WIDGET_H__
