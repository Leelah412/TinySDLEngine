#ifndef __SDL_DATA_H__
#define __SDL_DATA_H__

#include <SDL.h>
#include <gl/glew.h>
#include <gl/GLU.h>
#include <SDL_opengl.h>

#include <queue>
#include <deque>
#include <set>
#include <iostream>

#define USE_SDL_DATA extern SDLData* SDL_Data;

using namespace std;

// a "singleton" class containing variables to be used globally 
class SDLData {
public:
	SDLData();
	~SDLData();

	SDL_Window* get_window();
	void set_window(SDL_Window* window);
	SDL_GLContext get_context();
	void set_context(SDL_GLContext context);

private:
	SDL_Window* m_window;
	SDL_GLContext m_context;		// context is a void*
};


#endif