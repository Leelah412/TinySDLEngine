#ifndef __GAME_LOOP_H__
#define __GAME_LOOP_H__

#include <SDLData.h>
#include <nodes/Node.h>
#include <SDLEventManager.h>
#include <world/collision/CollisionHandler.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <renderer/Renderer.h>
#include <renderer/Buffer.h>
#include <renderer/Shader.h>
#include <renderer/Texture.h>

USE_EVENT_MANAGER
USE_DEFAULT_NODE_TREE
USE_COLLISION_HANDLER
USE_RENDERER
USE_RENDER_MANAGER

extern SDLData* SDL_Data;

class GameLoop : public SEMObject{

public:
	GameLoop();
	~GameLoop();

	// Handle interrupts (e.g. quit or pause commands)
	void handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext) override;

	// Initialize SDL
	int init(const char* title = "TinySDLEngine", int x = SDL_WINDOWPOS_UNDEFINED, int y = SDL_WINDOWPOS_UNDEFINED);

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
	int quit();						// Quit the application

	/* FPS */
	uint8_t get_max_fps();
	void set_max_fps(uint8_t fps);

	/* TIME GETTERS */
	time_t get_curtime();
	time_t get_prevtime();
	time_t get_delta();				// Return the difference between the times of the current and previous frame

private:
	bool m_initialized = false;		//
	bool m_run = false;				// Loop condition
	bool m_pause = false;			// Pause condition

	uint8_t m_max_fps = 60;			// Frames per second ceiling (60fps as default)
	bool m_vsync_enabled = false;

	time_t m_curtime = 0;			// Current frame time
	time_t m_prevtime = 0;			// Previous frame time
	time_t m_delta = 0;				// Delta between current and previous frame
	time_t m_pausetime = 0;			// Time at point of pausing the loop

	/* OPEN GL (TODO: variables to be refactored into an own renderer class) */

	bool init_gl();

	Texture* m_texture = nullptr;	// TODO: only temporary solution of course

};

#endif // !__GAME_LOOP_H__
