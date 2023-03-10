#include "app.h"

TSE_USE_APP(Dungeon, "examples/dungeon/config.json")

Dungeon::Dungeon(){}
Dungeon::~Dungeon(){}

void Dungeon::handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext){
	if(event == SEM_QUIT) stop();
	if((event == SEM_KEY_PRESSED) && sdl_event.key.keysym.sym == SDLK_ESCAPE) stop();
}

int Dungeon::init(){
	listen_event(SEM_KEY_PRESSED, SEM_EVENT_EXT(SDLK_ESCAPE));

	SDL_SetRelativeMouseMode(SDL_TRUE);

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

int Dungeon::quit(){
	drop_event(SEM_KEY_PRESSED);
	return 0;
}
