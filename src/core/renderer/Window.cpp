#include "Window.h"

namespace tse{

Window::Window(
	const std::string& title,
	int pos_x,
	int pos_y,
	int width,
	int height,
	Uint32 flags
): m_title{title}, m_pos_x{pos_x}, m_pos_y{pos_y}, m_width{width}, m_height{height}, m_flags{flags}
{
	m_window = SDL_CreateWindow(m_title.c_str(), m_pos_x, m_pos_y, m_width, m_height, m_flags);
}

Window::~Window(){
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
};

SDL_Window* Window::get_window() const{
	return m_window;
}
void Window::set_window(SDL_Window* window){
	m_window = window;
	SDL_GL_DeleteContext(m_context);
	create_context();
	// TODO: must initialize gl stuff too!
}

SDL_GLContext Window::get_context() const{
	return m_context;
}
void Window::create_context(){
	if(!m_window) return;
	m_context = SDL_GL_CreateContext(m_window);
}

std::string Window::title(){
	return m_title;
}
int Window::pos_x(){
	return m_pos_x;
}
int Window::pos_y(){
	return m_pos_y;
}

int Window::width(){
	return m_width;
}
void Window::set_width(int width){
	m_width = width;
	if(!m_window) return;
	SDL_SetWindowSize(m_window, m_width, m_height);
}

int Window::height(){
	return m_height;
}
void Window::set_height(int height){
	m_height = height;
	if(!m_window) return;
	SDL_SetWindowSize(m_window, m_width, m_height);
}

int Window::flags(){
	return m_flags;
}

bool Window::vsync_on(){
	return m_vsync;
}
void Window::toggle_vsync(){
	m_vsync = !m_vsync;
}
void Window::set_vsync(bool vsync){
	m_vsync = vsync;
}
Uint16 Window::max_fps(){
	return m_max_fps;
}
void Window::set_max_fps(Uint16 fps){
	m_max_fps = fps;
}
void Window::set_fps_30(){
	m_max_fps = 30;
}
void Window::set_fps_60(){
	m_max_fps = 60;
}


}