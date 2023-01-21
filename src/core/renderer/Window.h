#include <SDL.h>
#include <string>

namespace tse{
	class Window{

	public:
		Window(
			const std::string& title,
			uint16_t pos_x = SDL_WINDOWPOS_UNDEFINED,
			uint16_t pos_y = SDL_WINDOWPOS_UNDEFINED,
			uint16_t width = 1280,
			uint16_t height = 720,
			uint32_t flags = SDL_WINDOW_SHOWN
		) : m_title{title}, m_pos_x{pos_x}, m_pos_y{pos_y}, m_width{width}, m_height{height}, m_flags{flags}
		{
			m_window = SDL_CreateWindow(m_title.c_str(), m_pos_x, m_pos_y, m_width, m_height, m_flags);
		}
		~Window(){
			SDL_GL_DeleteContext(m_context);
			SDL_DestroyWindow(m_window);
		};

		SDL_Window* get_window() const{
			return m_window;
		}
		void set_window(SDL_Window* window){
			m_window = window;
			SDL_GL_DeleteContext(m_context);
			create_context();
			// TODO: must initialize gl stuff too!
		}

		SDL_GLContext get_context() const{
			return m_context;
		}
		void create_context(){
			if(!m_window) return;
			m_context = SDL_GL_CreateContext(m_window);
		}

		std::string title(){
			return m_title;
		}
		uint16_t pos_x(){
			return m_pos_x;
		}
		uint16_t pos_y(){
			return m_pos_y;
		}

		uint16_t width(){
			return m_width;
		}
		void set_width(uint16_t width){
			m_width = width;
			if(!m_window) return;
			SDL_SetWindowSize(m_window, m_width, m_height);
		}

		uint16_t height(){
			return m_height;
		}
		void set_height(uint16_t height){
			m_height = height;
			if(!m_window) return;
			SDL_SetWindowSize(m_window, m_width, m_height);
		}

		uint32_t flags(){
			return m_flags;
		}

		bool vsync_on(){
			return m_vsync;
		}
		void toggle_vsync(){
			m_vsync = !m_vsync;
		}
		void set_vsync(bool vsync){
			m_vsync = vsync;
		}
		uint16_t max_fps(){
			return m_max_fps;
		}
		void set_max_fps(uint16_t fps){
			m_max_fps = fps;
		}
		void set_fps_30(){
			m_max_fps = 30;
		}
		void set_fps_60(){
			m_max_fps = 60;
		}

	private:
		SDL_Window* m_window;
		SDL_GLContext m_context = nullptr;

		std::string m_title;
		uint16_t m_pos_x;
		uint16_t m_pos_y;
		uint16_t m_width;
		uint16_t m_height;
		uint32_t m_flags;

		bool m_vsync = false;
		uint16_t m_max_fps = -1;

		// (Re-)Initialize OpenGL after each context creation
		void init_gl(){

		}
	};
}

