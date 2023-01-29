#include "Editor.h"

namespace tse{

Editor::Editor(){

}

Editor::~Editor(){
}

int Editor::init(const std::string& config)
{
	return 0;
}

// TODO: remove
static bool open = true;
static bool show_demo_window = true;

void Editor::render(){
	IRenderer->clear();
	IRenderer->draw();

	// Scene is rendered in Application

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if(show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	if(open){
		static float f = 0.0f;

		ImGui::Begin("Hello, world!", &open);                          // Create a window called "Hello, world!" and append into it.

		ImGui::Checkbox("Show Demo", &show_demo_window);
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Render OpenGL on SDL Window
	SDL_GL_SwapWindow(get_window()->window());

}

void Editor::switch_scene(const std::string& path){
	SceneLoader::load_scene(path);
}


}
