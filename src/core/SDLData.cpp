#include "SDLData.h"

SDLData* SDL_Data = new SDLData();

SDLData::SDLData(){
	m_window = nullptr;
	m_context = nullptr;
}

SDLData::~SDLData(){
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
}


SDL_Window* SDLData::get_window(){
	return m_window;
}
void SDLData::set_window(SDL_Window* window){
	m_window = window;
}
SDL_GLContext SDLData::get_context(){
	return m_context;
}
void SDLData::set_context(SDL_GLContext context){
	m_context = context;
}