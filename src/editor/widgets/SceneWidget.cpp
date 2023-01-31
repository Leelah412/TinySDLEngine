#include "SceneWidget.h"

namespace tse{

SceneWidget::SceneWidget(int x, int y, int w, int h, PROJECTION proj_type) : Widget{x, y, w, h}, m_projection_type{proj_type}{
	// TODO: we don't add camera node as child of this Widget. why? because im not even sure, whether i want Widget to be a Node type or not
	m_editor_cam = new EditorCamera(w, h, proj_type);

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
	// TODO ASAP: implement framebuffer and bind camera to it!!
	m_editor_cam->activate_camera();
	IRenderer->draw();
	m_editor_cam->deactivate_camera();

	ImGui::End();
}


}
