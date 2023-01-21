#include "GameLoop.h"

#include "nodes/FirstPersonCamera.h"
#include "nodes/ModelNode.h"
#include "nodes/LightNode.h"
#include "renderer/VertexArray.h"
#include "renderer/Mesh.h"
#include "renderer/defaults.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <sstream>
#include <memory>

void gl_clear_error(){
	while(glGetError() != GL_NO_ERROR);
}
void gl_get_error(){
	while(GLenum error = glGetError()){
		std::cout << "OpenGL Error: " << std::hex << error << std::endl;
	}
}

// Adapt event manager to support ImGui
class SEMImGuiAdapter : public SDLEventManager{
	void handle_event(const SDL_Event& event) override{
		// GUI handles events first!
		ImGui_ImplSDL2_ProcessEvent(&event);

		// Do not notify listeners, if GUI ate event
		if( (event.type >= SDL_KEYDOWN) &&
			(event.type <= SDL_KEYMAPCHANGED) &&
			(ImGui::GetIO().WantCaptureKeyboard))
		{
			return;
		}
		else if((event.type >= SDL_MOUSEMOTION) &&
			(event.type <= SDL_MOUSEWHEEL) &&
			(ImGui::GetIO().WantCaptureMouse))
		{
			return;
		}

		notify_helper(event);
	}
};

GameLoop::GameLoop(){}

GameLoop::~GameLoop(){
	if(m_initialized) quit();
}

void GameLoop::handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext){
	if(event == SEM_QUIT) stop();
	if((event == SEM_KEY_PRESSED) && sdl_event.key.keysym.sym == SDLK_ESCAPE) stop();
}

int GameLoop::init(const char* title, int x, int y){
	using namespace tse;

	// Initialize event manager first!
	delete EventManager;
	EventManager = new SEMImGuiAdapter();
	// Listen to Quit interrupt to be able to close the program
	listen_event(SEM_QUIT);
	listen_event(SEM_KEY_PRESSED, SEM_EVENT_EXT(SDLK_ESCAPE));

	// TODO: remove
	// Initialize SDL Data 
	if(!SDL_Data) SDL_Data = new SDLData();

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		std::cout << SDL_GetError() << std::endl;
		return 1;
	}

	cout << "SDL initialized" << endl;

	// TODO: replace with "Window" class
	SDL_Data->set_window(SDL_CreateWindow(title, x, y, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN));
	if(!SDL_Data->get_window()){
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 3;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);

	cout << "SDL Window initialized" << endl;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// TODO: replace with "Window" class
	// Must create OpenGL context BEFORE initializing glew!!!
	SDL_Data->set_context(SDL_GL_CreateContext(SDL_Data->get_window()));
	if(!SDL_Data->get_context()){
		printf("OpenGL Context could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 4;
	}

	// Initialize Node Tree
	if(!INodeTree) SetNodeTree(new NodeTree());
	// Initialize Collision Handler
	if(!ICollisionHandler) SetCollisionHandler(new CollisionHandler());

	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if(glewError != GLEW_OK){
		printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		SDL_Quit();
		return 5;
	}

	cout << glGetString(GL_VERSION) << endl;

	//Use Vsync
	if(SDL_GL_SetSwapInterval(1) < 0){
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}

	cout << "VSync enabled" << std::endl;

	//Initialize OpenGL
	if(!init_gl())	{
		printf("Unable to initialize OpenGL!\n");
		SDL_Quit();
		return 6;
	}

	cout << "OpenGL initialized" << endl;

	// Initialize ImGui
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(SDL_Data->get_window(), SDL_Data->get_context());
	ImGui_ImplOpenGL3_Init("#version 460");

	cout << "ImGui initialized" << endl;

	// TODO: connect collision handler with static parent/child mutation signals of physics object,
	// such that the collision handler lists get updated any time a physics object gets added to/removed from the node tree

	m_initialized = true;

	std::cout << "Application initialized" << std::endl;

	return 0;
}

// updates the gamestates for all objects
void GameLoop::update(const time_t& delta_time){
	using namespace tse;

	// call collision handler first
	ICollisionHandler->update(m_delta);

	deque<Node*> obj_queue;
	obj_queue.push_front(INodeTree->get_root_node());
	Node* front = nullptr;

	// TODO: with the current implementation it is possible, that e.g. an updated node assigns a new child node to a child node,
	// meaning that when the child node gets updated, it pushes the newly created node from the most recent frame into the queue
	// and allows it to get updated too, while a new node assigned as a child to a node of a lower grade will never get the same treatment.
	// To counter this, we must not allow any newly added node to be updated during the current frame!
	// 
	// unlike in render(), we don't want to immediately update every node,
	// but instead assign every tree object into the queue and
	while(!obj_queue.empty()){
		front = obj_queue.front();
		obj_queue.pop_front();
		// push every child object of "front" into the queue
		for(auto ch : front->get_children()){
			obj_queue.push_front(ch);
		}
		// update the object
		front->update(delta_time);
	}
}

// pass down events through the object tree, until an object "eats" it
void GameLoop::input(){
	// just let event manager do everything
	EventManager->poll_events();
}

// renders all objects
void GameLoop::render(){

	//m_vertex_array->get_buffer_object((unsigned int)0)->get_shader()->set_uniform_4f("u_color", 0.8, 0.8, 0.2, 1.0);

	IRenderer->clear();
	IRenderer->draw();

	/*
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
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
	*/

	// Render OpenGL on SDL Window
	SDL_GL_SwapWindow(SDL_Data->get_window());	
}

int GameLoop::loop(){

	while(m_run && !m_pause){
		m_curtime = SDL_GetTicks();
		// always check for input
		input();
		// update, if delta is above fps
		if( (m_delta = m_curtime - m_prevtime) > (1000 / m_max_fps) ){
			m_prevtime = m_curtime;
			update(m_delta);
			render();
		}
	}

	return true;
}

int GameLoop::start(){
	// Game is already running, no need to do the loop twice
	if(m_run) return false;

	/* INITIALIZATION */
	if(!m_initialized) return -1;

	m_run = true;
	m_curtime = 0;
	m_prevtime = SDL_GetTicks();

	/* GAME LOOP */

	cout << "Starting game loop" << endl;
	loop();

	return true;
}
int GameLoop::stop(){
	cout << "Stopping game loop" << endl;
	m_run = false;
	return true;
}
int GameLoop::pause(){
	cout << "Pausing game loop" << endl;
	m_pause = true;
	return true;
}
int GameLoop::resume(){
	cout << "Resuming game loop" << endl;
	m_pause = false;
	return true;
}
int GameLoop::quit(){

	std::cout << "Quitting game loop" << std::endl;

	// Stop ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	drop_event(SEM_QUIT);
	drop_event(SEM_KEY_PRESSED);

	//delete SDL_Data;
	delete ICollisionHandler;

	SDL_Quit();
	
	m_initialized = false;
	return 0;
}

uint8_t GameLoop::get_max_fps(){
	return m_max_fps;
}
void GameLoop::set_max_fps(uint8_t fps){
	m_max_fps = fps;
}

time_t GameLoop::get_curtime(){
	return m_curtime;
}
time_t GameLoop::get_prevtime(){
	return m_prevtime;
}
time_t GameLoop::get_delta(){
	return m_delta;
}

bool GameLoop::init_gl(){

	using namespace tse;

	/* Initialize default VAOs and buffer entities */ 
	// TODO: create default framebuffers

	// Default renderer setup
	IRenderer->setup();
	// Create default shader
	// TODO: make some kind of static "Settings" interface, where we save basic settings for loading stuff, e.g. default shader
	IRenderManager->create_default_shader("src/res/shader/shader.glsl", false);
	IRenderManager->bind(IRenderManager->get_default_shader());
	IModelManager->init_world();
	// Push ModelManager render operation to render list
	// TODO: create default mesh render operation, which can be overwritten by others, and push that into the list
	IRenderManager->add_render_operation(new MeshRender());

	/* TEST */
#if 0
	float vertices[] = {
			-0.5f,0.5f,-0.5f,	0,0, 0,0,0,
			-0.5f,-0.5f,-0.5f,	0,1, 0,1,0,
			0.5f,-0.5f,-0.5f,	1,1, 1,1,1,
			0.5f,0.5f,-0.5f,	1,0, 1,0,1,
									 
			-0.5f,0.5f,0.5f,	0,0, 0,0,0,
			-0.5f,-0.5f,0.5f,	0,1, 0,1,0,
			0.5f,-0.5f,0.5f,	1,1, 1,1,1,
			0.5f,0.5f,0.5f,		1,0, 1,0,1,
									 
			0.5f,0.5f,-0.5f,	0,0, 0,0,0,
			0.5f,-0.5f,-0.5f,	0,1, 0,1,0,
			0.5f,-0.5f,0.5f,	1,1, 1,1,1,
			0.5f,0.5f,0.5f,		1,0, 1,0,1,
									 
			-0.5f,0.5f,-0.5f,	0,0, 0,0,0,
			-0.5f,-0.5f,-0.5f,	0,1, 0,1,0,
			-0.5f,-0.5f,0.5f,	1,1, 1,1,1,
			-0.5f,0.5f,0.5f,	1,0, 1,0,1,
									 
			-0.5f,0.5f,0.5f,	0,0, 0,0,0,
			-0.5f,0.5f,-0.5f,	0,1, 0,1,0,
			0.5f,0.5f,-0.5f,	1,1, 1,1,1,
			0.5f,0.5f,0.5f,		1,0, 1,0,1,
									 
			-0.5f,-0.5f,0.5f,	0,0, 0,0,0,
			-0.5f,-0.5f,-0.5f,	0,1, 0,1,0,
			0.5f,-0.5f,-0.5f,	1,1, 1,1,1,
			0.5f,-0.5f,0.5f,	1,0, 1,1,0

	};
	std::vector<GLuint> indices = {
			0,1,3,
			3,1,2,
			4,5,7,
			7,5,6,
			8,9,11,
			11,9,10,
			12,13,15,
			15,13,14,
			16,17,19,
			19,17,18,
			20,21,23,
			23,21,22
	};
#endif

	// TEST load nodes
	// Create test cube with default material
	Material* def_mat = new Material(IRenderManager->get_default_shader());
	// note: do NOT create new textures with the material, as it won't be tracked by resource manager
	// in case of a texture change, the given texture will NOT be deleted!
	// therfore set texture MUST already exist, if we want at least the possibility of avoiding a mem leak!
	def_mat->set_texture("u_texture", new Texture("src/res/textures/example.PNG"));
	//Mesh* mesh = new Mesh((const void*)vertices, sizeof(vertices), indices);
	//Model* model = new Model(mesh);
	Model* model = new Model("src/res/mesh/Prop_Boat_1.obj");
	model->assign_material(*model->get_mesh()->get_submesh_list().begin(), def_mat);

	ModelNode* parent = new ModelNode(model);
	FirstPersonCamera* camnode = new FirstPersonCamera(tse::PERSPECTIVE, 640.0f, 480.0f);
	
	camnode->set_unique_name("fps_camera");
	camnode->set_position(glm::vec3(-2.0f, 2.0f, 0.0f));
	camnode->set_pitch(45.0f);
	INodeTree->get_root_node()->add_child(parent);
	INodeTree->get_root_node()->add_child(camnode);
	
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
	lightnode->turn_on();
	camnode->add_child(lightnode);
	
	// render on screen using camera of camnode
	camnode->activate_camera();

	return true;
}