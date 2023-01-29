#include <Application.h>

#include "nodes/FirstPersonCamera.h"
#include "nodes/ModelNode.h"
#include "nodes/LightNode.h"
#include "renderer/VertexArray.h"
#include "renderer/Mesh.h"
#include "renderer/defaults.h"
#include "util/ObjLoader.h"

using namespace tse;

// TODO: move to src again, but keep config in project folder
// -- when doing actual projects with sandboxes "seperate" from the engine, each sandbox will have a single "app.cpp" file,
// -- which will be invoked any time the game is supposed to run

int main(int argc, char* args[]){

	// Initializes the application
	TSE_APPLICATION_INIT("examples/test/config.json");

	// Put your scene initializer code here

#define testload

#ifndef testload
	// create mesh
	ObjLoader::create_model_from_obj("examples/test/res/mesh/Prop_Boat_1.obj");
	ObjLoader::create_model_from_obj("examples/test/res/mesh/Prop_Barrel_1.obj");
	ObjLoader::create_model_from_obj("examples/test/res/mesh/Waterwheel_1.obj");

	ObjLoader::create_model_from_obj("examples/test/res/mesh/Stone_Wall_1.obj");
	ObjLoader::create_model_from_obj("examples/test/res/mesh/Stone_Wall_2.obj");
	ObjLoader::create_model_from_obj("examples/test/res/mesh/Stone_Wall_3.obj");
	ObjLoader::create_model_from_obj("examples/test/res/mesh/Stone_Wall_4.obj");
	ObjLoader::create_model_from_obj("examples/test/res/mesh/Stone_Wall_5.obj");

	// load nodes
	// Create test cube with default material
	//Material* def_mat = new Material(IRenderManager->get_default_shader());
	// note: do NOT create new textures with the material, as it won't be tracked by resource manager
	// in case of a texture change, the given texture will NOT be deleted!
	// therfore set texture MUST already exist, if we want at least the possibility of avoiding a mem leak!
	//def_mat->set_texture("u_texture", new Texture("src/res/textures/example.PNG"));

	// models
	ModelNode* boat = new ModelNode("examples/test/res/mesh/Prop_Boat_1.obj.model");
	boat->set_unique_name("boat");
	ModelNode* barrel = new ModelNode("examples/test/res/mesh/Prop_Barrel_1.obj.model");
	barrel->set_unique_name("barrel");
	barrel->set_position_x(-2.0f);
	ModelNode* wheel = new ModelNode("examples/test/res/mesh/Waterwheel_1.obj.model");
	wheel->set_unique_name("wheel");
	wheel->set_position_x(2.0f);

	ModelNode* wall1 = new ModelNode("examples/test/res/mesh/Stone_Wall_1.obj.model");
	wall1->set_unique_name("wall1");
	wall1->set_position_x(-3.0f);
	wall1->set_position_z(1.0f);
	ModelNode* wall2 = new ModelNode("examples/test/res/mesh/Stone_Wall_2.obj.model");
	wall2->set_unique_name("wall2");
	wall2->set_position_x(-3.0f);
	wall2->set_position_z(-1.0f);
	ModelNode* wall3 = new ModelNode("examples/test/res/mesh/Stone_Wall_3.obj.model");
	wall3->set_unique_name("wall3");
	wall3->set_position_x(-3.0f);
	wall3->set_position_z(2.0f);
	ModelNode* wall4 = new ModelNode("examples/test/res/mesh/Stone_Wall_4.obj.model");
	wall4->set_unique_name("wall4");
	wall4->set_position_x(-3.0f);
	wall4->set_position_z(-2.0f);
	ModelNode* wall5 = new ModelNode("examples/test/res/mesh/Stone_Wall_5.obj.model");
	wall5->set_unique_name("wall5");
	wall5->set_position_x(-3.0f);

	// camera
	FirstPersonCamera* camnode = new FirstPersonCamera(PERSPECTIVE, Config::window_w, Config::window_h);
	camnode->set_unique_name("fps_camera");
	camnode->set_position(glm::vec3(-2.0f, 2.0f, 0.0f));
	camnode->set_pitch(45.0f);

	// creating tree
	INodeTree->get_root_node()->add_child(boat);
	INodeTree->get_root_node()->add_child(barrel);
	INodeTree->get_root_node()->add_child(wheel);
	INodeTree->get_root_node()->add_child(wall1);
	INodeTree->get_root_node()->add_child(wall2);
	INodeTree->get_root_node()->add_child(wall3);
	INodeTree->get_root_node()->add_child(wall4);
	INodeTree->get_root_node()->add_child(wall5);
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
	SceneLoader::save_scene("examples/test/res/test.json");
#endif

#ifdef testload
	SceneLoader::load_scene("examples/test/res/test.json");
	// compare and check, if scene is loaded correctly, i.e. loaded scene outputs the same as saved scene
	SceneLoader::save_scene("examples/test/res/test_load_comp.json");
#endif

	// Starts the game
	TSE_APPLICATION_RUN;


	return 0;
}