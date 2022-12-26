#include "GameLoop.h"

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

GameLoop::GameLoop(){

}

GameLoop::~GameLoop(){
	if(m_initialized) quit();
}

void GameLoop::handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext){
	if(event == SEM_QUIT) stop();
	if((event == SEM_KEY_PRESSED) && sdl_event.key.keysym.sym == SDLK_ESCAPE) stop();
}

int GameLoop::init(const char* title, int x, int y){

	// Initialize event manager first!
	EventManager = new SEMImGuiAdapter();
	// Listen to Quit interrupt to be able to close the program
	listen_event(SEM_QUIT);
	listen_event(SEM_KEY_PRESSED, SEM_EVENT_EXT(SDLK_ESCAPE));

	// Initialize SDL Data 
	if(!SDL_Data) SDL_Data = new SDLData();

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		std::cout << SDL_GetError() << std::endl;
		return 1;
	}
	/*
	if(IMG_Init(IMG_INIT_PNG) != 2){
		std::cout << IMG_GetError() << std::endl;
		return 2;
	}
	*/

	SDL_Data->set_window(SDL_CreateWindow(title, x, y, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN));
	if(!SDL_Data->get_window()){
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 3;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Must create OpenGL context BEFORE initializing glew!!!
	SDL_Data->set_context(SDL_GL_CreateContext(SDL_Data->get_window()));
	if(!SDL_Data->get_context()){
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

	/*
	//Use Vsync
	if(SDL_GL_SetSwapInterval(1) < 0){
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
	}
	*/

	//Initialize OpenGL
	if(!init_gl())
	{
		printf("Unable to initialize OpenGL!\n");
		SDL_Quit();
		return 6;
	}

	printf("OpenGL initialized\n");

	// Initialize ImGui
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForOpenGL(SDL_Data->get_window(), SDL_Data->get_context());
	ImGui_ImplOpenGL3_Init("#version 460");


	// Initialize Node Tree
	if(!INodeTree) INodeTree = new NodeTree();
	// Initialize Collision Handler
	if(!ICollisionHandler) ICollisionHandler = new CollisionHandler();

	// TODO: connect collision handler with static parent/child mutation signals of physics object,
	// such that the collision handler lists get updated any time a physics object gets added to/removed from the node tree

	m_initialized = true;

	cout << "SDL initialized" << endl;

	return 0;
}

// updates the gamestates for all objects
void GameLoop::update(const time_t& delta_time){
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

float r = 0.5f;
float inc = 0.05f;

// TODO: delete, example!
// Our state
bool show_demo_window = true;
bool open = true;

// renders all objects
void GameLoop::render(){

	if(r > 1.0f) inc = -0.05f;
	else if(r < 0.0f) inc = 0.05f;
	r += inc;

	m_shader->bind();
	m_shader->set_uniform_4f("u_color", r, 0.1f, 0.8f, 1.0f);

	Renderer::render(*m_vertex_array, *m_index_buffer, *m_shader);

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

	// Render OpenGL on SDL Window
	SDL_GL_SwapWindow(SDL_Data->get_window());

	/*
		//Bind program
		glUseProgram(m_program_id);
		//Enable vertex position
		glEnableVertexAttribArray(m_vertex_pos_2d_location);

		//Set vertex data
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexAttribPointer(m_vertex_pos_2d_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

		//Set index data and render
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

		//Disable vertex position
		glDisableVertexAttribArray(m_vertex_pos_2d_location);

		//Unbind program
		glUseProgram(NULL);
	*/
	
}

int GameLoop::loop(){

	while(m_run && !m_pause){
		m_curtime = SDL_GetTicks();
		// always check for input
		input();
		// update, if delta is above fps
		if( (m_delta = m_curtime - m_prevtime) > (1000 / m_max_fps) ){
			m_prevtime = m_curtime;
			//update(m_delta);
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
	delete SDL_Data;
	delete ICollisionHandler;

	// TODO: when moving to renderer, move following lines to the deconstructor or whatever of the renderer
	delete m_vertex_array;
	delete m_index_buffer;
	delete m_shader;

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

	// Enable 2D Textures
	glEnable(GL_TEXTURE_2D);
	// Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* TEST */

	// Vertex array containing 2D positions
	float tri[] = {
		//       x,    y
			-0.5f, -0.5f, 0.0f, 0.0f,
			 0.5f, -0.5f, 1.0f, 0.0f,
			 0.5f,  0.5f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 1.0f
	};
	// Index array containing a list of triangles
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	// TODO: maybe add some kind of reference manager, which tracks all pointers given to it
	// and deletes them at the end of the program, if they haven't been deleted
	m_vertex_array = new VertexArray(); 
	VertexBuffer* vb = new VertexBuffer(tri, 4 * 4 * sizeof(float));
	m_index_buffer = new IndexBuffer(indices, 6);

	BufferLayout layout = BufferLayout();
	layout.push(2, GL_FLOAT, true);
	layout.push(2, GL_FLOAT, true);
	m_vertex_array->add_buffer(vb, layout);

	m_shader = new Shader("src/res/shader/shader.glsl");
	// Use the shader
	m_shader->bind();
	// bottom is negative, since opengl uses your usual coordinate system
	glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-0.4f, 0.0f, 0.0f));
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0f));
	m_shader->set_uniform_mat4f("u_proj", proj * view * model);

	m_texture = new Texture("src/res/textures/karte.PNG");
	m_texture->bind();
	m_shader->set_uniform_1i("u_texture", 0);

	/*
	// Graphics program
	m_program_id = glCreateProgram();

	//Create vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	//Get vertex source
	const GLchar* vertexShaderSource[] =
	{
		"#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"
	};

	//Set vertex source
	glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

	//Compile vertex source
	glCompileShader(vertexShader);

	//Check vertex shader for errors
	GLint vShaderCompiled = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
	if(vShaderCompiled != GL_TRUE){
		printf("Unable to compile vertex shader %d!\n", vertexShader);
		return false;
	}

	//Attach vertex shader to program
	glAttachShader(m_program_id, vertexShader);

	//Create fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	//Get fragment source
	const GLchar* fragmentShaderSource[] =
	{
		"#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"
	};

	//Set fragment source
	glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

	//Compile fragment source
	glCompileShader(fragmentShader);

	//Check fragment shader for errors
	GLint fShaderCompiled = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
	if(fShaderCompiled != GL_TRUE)	{
		printf("Unable to compile fragment shader %d!\n", fragmentShader);
		return false;
	}

	//Attach fragment shader to program
	glAttachShader(m_program_id, fragmentShader);

	//Link program
	glLinkProgram(m_program_id);

	//Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(m_program_id, GL_LINK_STATUS, &programSuccess);
	if(programSuccess != GL_TRUE)
	{
		printf("Error linking program %d!\n", m_program_id);
		return false;
	}
	//Get vertex attribute location
	m_vertex_pos_2d_location = glGetAttribLocation(m_program_id, "LVertexPos2D");
	if(m_vertex_pos_2d_location == -1)
	{
		printf("LVertexPos2D is not a valid glsl program variable!\n");
		return false;
	}

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//VBO data
	GLfloat vertexData[] =
	{
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f
	};

	//IBO data
	GLuint indexData[] = {0, 1, 2, 3};

	//Create VBO
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

	//Create IBO
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);
	
	*/

	return true;
}