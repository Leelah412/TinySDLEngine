#include "SceneWidget.h"

namespace tse{

SceneWidget::SceneWidget(int x, int y, int w, int h, PROJECTION proj_type) : Widget{x, y, w, h}, m_projection_type{proj_type}{
	// TODO: we don't add camera node as child of this Widget. why? because im not even sure, whether i want Widget to be a Node type or not
	unsigned int _w = (w >= 0 ? w : Config::window_w), _h = (h >= 0 ? h : Config::window_h);
	m_editor_cam = new EditorCamera(_w, _h, m_projection_type);
	m_framebuffer = new Framebuffer(_w, _h, TSE_FBO_COLOR_TARGET);
	m_editor_cam->get_camera()->set_framebuffer(m_framebuffer->get_buffer_id());

	// TODO: FOR TEST PURPOSES, REMOVE!
	SceneLoader::load_scene("examples/dungeon/res/scenes/dungeon.json");

}

SceneWidget::~SceneWidget(){
	delete m_editor_cam;
}

void SceneWidget::update(const time_t& delta){

}

void SceneWidget::draw(){
	if(!ImGui::Begin("Scene", &m_show)) return;

	// Each Scene viewport shall only capture from a single camera instance, so activate camera before and deactivate after rendering every time
	m_editor_cam->activate_camera();
	m_framebuffer->bind();
	IRenderer->draw();
	m_framebuffer->unbind();
	m_editor_cam->deactivate_camera();

	// Using a Child allow to fill all the space of the window.
	// It also alows customization
	ImGui::BeginChild("GameRender");
	// Get the size of the child (i.e. the whole draw size of the windows).
	ImVec2 wsize = ImGui::GetWindowSize();
	// Because I use the texture from OpenGL, I need to invert the V from the UV.
	ImGui::Image((ImTextureID)m_framebuffer->get_color_target()->get_texture_id(), wsize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();

	ImGui::End();
}


}
