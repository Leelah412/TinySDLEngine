#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "Application.h"

#include "nodes/Node.h"
#include "SceneLoader.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <pugixml/pugixml.hpp>

#include <string>
#include <map>
#include <iostream>
#include <thread>

namespace tse{

class Editor : public Application{

public:
	Editor();
	virtual ~Editor();

	// Initialize the editor
	// Takes a path to the Editor configuration XML file
	int init(const std::string& config) override;
	// Render the editor on the window
	void render() override;
	// Execute the game, starting at the entry scene specified in the config file
	//void run_game();
	// Pause the currently running game
	//void pause_game();
	// Stop the currently running game
	//void stop_game();
	// Open/Focus on a different scene tree
	void switch_scene(const std::string& path);

private:
	std::string m_cur_scene;						// Path to the current scene
	std::map<std::string, Node*> m_scene_trees;		// List of scene trees currently opened in the editor; key is path to/name of scene
};

}


#endif // !__EDITOR_H__
