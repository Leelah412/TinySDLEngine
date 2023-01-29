#include <Application.h>

#include "nodes/FirstPersonCamera.h"
#include "nodes/ModelNode.h"
#include "nodes/LightNode.h"
#include "res/nodes/WorldGenerator.h"
#include "util/ObjLoader.h"

using namespace tse;

int main(int argc, char* args[]){

	// Initializes the application
	TSE_APPLICATION_INIT("examples/dungeon/config.json");

	// Put your scene initializer code here

#define testload

#ifndef testload
	// world generator
	WorldGenerator* world = new WorldGenerator();
	world->set_unique_name("world");
	world->generate();

	// camera
	FirstPersonCamera* camnode = new FirstPersonCamera(PERSPECTIVE, Config::window_w, Config::window_h);
	camnode->set_unique_name("fps_camera");
	camnode->set_position(glm::vec3(-24.0f, 24.0f, 0.0f));
	camnode->set_pitch(45.0f);

	// creating tree
	INodeTree->get_root_node()->add_child(world);
	INodeTree->get_root_node()->add_child(camnode);
	INodeTree->get_root_node()->set_unique_name("root");

	// add light
	Light light = Light();
	light.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light.direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	light.ambient = glm::vec4(0.4);
	light.diffuse = glm::vec4(0.2);
	light.specular = glm::vec4(0.4);
	light.constant = 1.0f;
	light.linear = 0.09f;
	light.quadratic = 0.032f;
	LightNode* lightnode = new LightNode(light);
	lightnode->set_unique_name("light");
	lightnode->turn_on();
	camnode->add_child(lightnode);

	// render on screen using camera of camnode
	camnode->activate_camera();

	// For testing purposes, call the SceneLoader save function
	SceneLoader::save_scene("examples/dungeon/res/test.json");
#endif

#ifdef testload
	SceneLoader::load_scene("examples/dungeon/res/test.json");
	// compare and check, if scene is loaded correctly, i.e. loaded scene outputs the same as saved scene
	SceneLoader::save_scene("examples/dungeon/res/test_load_comp.json");
#endif

	// Starts the game
	TSE_APPLICATION_RUN;


	return 0;
}