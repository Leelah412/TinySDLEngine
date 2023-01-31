#include <Application.h>

#include "nodes/FirstPersonCamera.h"
#include "nodes/ModelNode.h"
#include "nodes/LightNode.h"
#include "res/nodes/WorldGenerator.h"
#include "util/ObjLoader.h"

using namespace tse;

// TODO: replaced by app.h, remove

int main(int argc, char* args[]){

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
	light.spread = 5.0f;
	LightNode* lightnode = new LightNode(light);
	lightnode->set_unique_name("light");
	lightnode->turn_on();
	camnode->add_child(lightnode);

	// render on screen using camera of camnode
	camnode->activate_camera();

	// For testing purposes, call the SceneLoader save function
	SceneLoader::save_scene("examples/dungeon/res/scenes/dungeon.json");
#endif

#ifdef testload
	SceneLoader::load_scene("examples/dungeon/res/scenes/dungeon.json");
#endif


	return 0;
}