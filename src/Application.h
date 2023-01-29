#ifndef __GAME_LOOP_H__
#define __GAME_LOOP_H__

#include "util/SDLEventManager.h"
#include "util/Config.h"
#include "nodes/Node.h"
#include "world/collision/CollisionHandler.h"
#include "renderer/Window.h"
#include "renderer/Renderer.h"
#include "renderer/ModelManager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <json/single_include/nlohmann/json.hpp>

#include <string>
#include <fstream>

#define TSE_APPLICATION_INIT(config) \
	tse::Application* app = new tse::Application(); \
	if(app->init(config) != 0){ \
		return -1;				\
	}

#define TSE_APPLICATION_RUN \
	app->start(); \
	app->quit(); \
	delete app;

USE_EVENT_MANAGER
USE_RENDERER
USE_RENDER_MANAGER

namespace tse{

class Application : public SEMObject{

public:
	Application();
	~Application();

	// Handle interrupts (e.g. quit or pause commands)
	void handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext) override;

	// Initialize Application
	int init(const std::string& config_path);

	/* MAIN LOOP */
	int loop();

	void update(const time_t& delta_time);
	void input();
	void render();

	/* LOOP COMMANDS */
	int start();
	int stop();
	int pause();
	int resume();
	int quit();							// Quit the application

	/* TODO: move to Config.h FPS */
	uint8_t get_max_fps();
	void set_max_fps(uint8_t fps);

	/* TIME GETTERS */
	time_t get_curtime();
	time_t get_prevtime();
	time_t get_delta();					// Return the difference between the times of the current and previous frame

private:
	bool m_initialized = false;			//
	bool m_run = false;					// Loop condition
	bool m_pause = false;				// Pause condition

	time_t m_curtime = 0;				// Current frame time
	time_t m_prevtime = 0;				// Previous frame time
	time_t m_delta = 0;					// Delta between current and previous frame
	time_t m_pausetime = 0;				// Time at point of pausing the loop

	Window* m_window = nullptr;			// Window, on which the loop runs

	/* OPEN GL (TODO: variables to be refactored into an own renderer class) */

	bool init_gl();
};

}



#endif // !__GAME_LOOP_H__
