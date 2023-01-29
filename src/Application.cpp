#include "Application.h"

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

namespace tse{

Application::Application(){}

Application::~Application(){
	if(m_initialized) quit();
}

void Application::handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext){
	if(event == SEM_QUIT) stop();
	if((event == SEM_KEY_PRESSED) && sdl_event.key.keysym.sym == SDLK_ESCAPE) stop();
}

int Application::init(const std::string& config_path){
	// Config file must exist!
	std::ifstream file(config_path);
	if(!file.is_open()){
		std::cout << "ERROR: Config file at '" << config_path << "' doesn't exist! Abort." << std::endl;
		return -1;
	}
	JSON config = JSON::parse(file);
	file.close();

	apply_config(config);

	// Initialize event manager first!
	delete EventManager;
	EventManager = new SEMImGuiAdapter();
	// Listen to Quit interrupt to be able to close the program
	listen_event(SEM_QUIT);
	listen_event(SEM_KEY_PRESSED, SEM_EVENT_EXT(SDLK_ESCAPE));

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		std::cout << SDL_GetError() << std::endl;
		return 1;
	}

	std::cout << "SDL initialized" << std::endl;

	// TODO: put SDL flags in config too!
	m_window = new Window(Config::title, Config::window_x, Config::window_y, Config::window_w, Config::window_h, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if(!m_window->window()){
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 3;
	}

	// Initialize Node Tree
	if(!INodeTree) SetNodeTree(new NodeTree());
	// Initialize Collision Handler
	if(!ICollisionHandler) SetCollisionHandler(new CollisionHandler());

	SDL_SetRelativeMouseMode(SDL_TRUE);

	cout << "SDL Window initialized" << endl;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Must create OpenGL context BEFORE initializing glew!!!
	m_window->create_context();
	if(!m_window->context()){
		printf("OpenGL Context could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 4;
	}

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
	// TODO: per default, don't use imgui in base class Application, so remove!
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(m_window->window(), m_window->context());
	ImGui_ImplOpenGL3_Init("#version 420");

	cout << "ImGui initialized" << endl;

	m_initialized = true;

	std::cout << "Application initialized" << std::endl;

	return 0;
}

// updates the gamestates for all objects
void Application::update(const time_t& delta_time){
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
void Application::input(){
	// just let event manager do everything
	EventManager->poll_events();
}

// renders all objects
void Application::render(){
	IRenderer->clear();
	IRenderer->draw();

	// Render OpenGL on SDL Window
	SDL_GL_SwapWindow(m_window->window());
}

int Application::loop(){

	while(m_run && !m_pause){
		m_curtime = SDL_GetTicks();
		// always check for input
		input();
		// update, if delta is above fps
		if((m_delta = m_curtime - m_prevtime) > (1000 / Config::max_fps)){
			m_prevtime = m_curtime;
			update(m_delta);
			render();
		}
	}

	return true;
}

int Application::start(){
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
int Application::stop(){
	cout << "Stopping game loop" << endl;
	m_run = false;
	return true;
}
int Application::pause(){
	cout << "Pausing game loop" << endl;
	m_pause = true;
	return true;
}
int Application::resume(){
	cout << "Resuming game loop" << endl;
	m_pause = false;
	return true;
}
int Application::quit(){

	std::cout << "Quitting game loop" << std::endl;

	// Stop ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	drop_event(SEM_QUIT);
	drop_event(SEM_KEY_PRESSED);

	delete ICollisionHandler;
	delete INodeTree;
	delete m_window;

	SDL_Quit();

	m_initialized = false;
	return 0;
}

time_t Application::get_curtime(){
	return m_curtime;
}
time_t Application::get_prevtime(){
	return m_prevtime;
}
time_t Application::get_delta(){
	return m_delta;
}

const Window* Application::get_window() const{
	return m_window;
}

bool Application::init_gl(){
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

	return true;
}

}

