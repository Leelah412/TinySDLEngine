#include "Editor.h"


namespace tse{

#ifdef TSE_USE_EDITOR
	Application* create_application(){
		Editor* editor = new Editor();
		editor->set_config_path("src/editor/config.json");
		return editor;
	}
#endif // TSE_USE_EDITOR


Editor::Editor(){

}

Editor::~Editor(){
}

int Editor::init(){
	// Initialize ImGui
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(get_window()->window(), get_window()->context());
	ImGui_ImplOpenGL3_Init("#version 420");
	
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	cout << "ImGui initialized" << endl;

	// TODO: read XML file to create GUI
	m_widgets.insert(new ToolbarWidget());
	m_widgets.insert(new SceneTreeWidget());
	m_widgets.insert(new SceneWidget(Config::window_w, Config::window_h));

	return 0;
}

void Editor::render(){

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
	for(auto w : m_widgets){
		w->render();
	}

	IRenderer->clear();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Render OpenGL on SDL Window
	SDL_GL_SwapWindow(get_window()->window());

}

void Editor::switch_scene(const std::string& path){
	SceneLoader::load_scene(path);
}


}
