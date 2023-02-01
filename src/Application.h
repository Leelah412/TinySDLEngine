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

#ifndef TSE_USE_EDITOR
	#define TSE_USE_APP(appname, config) \
		tse::Application* tse::create_application(){ \
			tse::Application* app = new appname();	\
			app->set_config_path(config);	\
			return new appname();	\
		}
#else
	#define TSE_USE_APP(appname, config)
#endif

USE_EVENT_MANAGER
USE_RENDERER
USE_RENDER_MANAGER

namespace tse{

// Abstract class 
class Application : public SEMObject{

public:
	Application();
	virtual ~Application() = 0;

	// Handle interrupts (e.g. quit or pause commands)
	virtual void handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext) override;

	int run();

	// Configure Application
	virtual void config();
	// Initialize Application
	virtual int init();

	/* MAIN LOOP */
	virtual int loop();

	virtual void update(const time_t& delta_time);
	virtual void input();
	virtual void render();

	/* LOOP COMMANDS */
	virtual int start();
	virtual int stop();
	virtual int pause();
	virtual int resume();
	virtual int quit();							// Quit the application

	/* TIME GETTERS */
	time_t get_curtime();
	time_t get_prevtime();
	time_t get_delta();					// Return the difference between the times of the current and previous frame

	/* WINDOW */
	const Window* get_window() const;

	const std::string& get_config_path();
	void set_config_path(const std::string& path);

private:
	bool m_initialized = false;			//
	bool m_run = false;					// Loop condition
	bool m_pause = false;				// Pause condition

	time_t m_curtime = 0;				// Current frame time
	time_t m_prevtime = 0;				// Previous frame time
	time_t m_delta = 0;					// Delta between current and previous frame
	time_t m_pausetime = 0;				// Time at point of pausing the loop

	Window* m_window = nullptr;			// Window, on which the Application runs

	std::string m_config_path = "";		// Path to the configuration file

	/* OPEN GL (TODO: variables to be refactored into an own renderer class) */

	bool init_gl();

	// Basic initialization for all apps
	int base_init();
	// Basic quit instruction for all apps
	int base_quit();
};

Application* create_application();

}



#endif // !__GAME_LOOP_H__
