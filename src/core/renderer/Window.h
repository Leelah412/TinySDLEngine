#include <SDL.h>
#include <string>

namespace tse{

class Window{

public:
	Window(
		const std::string& title,
		int pos_x = SDL_WINDOWPOS_UNDEFINED,
		int pos_y = SDL_WINDOWPOS_UNDEFINED,
		int width = 1280,
		int height = 720,
		Uint32 flags = SDL_WINDOW_SHOWN
	);

	~Window();

	SDL_Window* get_window() const;
	void set_window(SDL_Window* window);

	SDL_GLContext get_context() const;
	void create_context();

	std::string title();
	int pos_x();
	int pos_y();

	int width();
	void set_width(int width);

	int height();
	void set_height(int height);

	int flags();

	bool vsync_on();
	void toggle_vsync();
	void set_vsync(bool vsync);
	Uint16 max_fps();
	void set_max_fps(Uint16 fps);
	void set_fps_30();
	void set_fps_60();

private:
	SDL_Window* m_window;
	SDL_GLContext m_context = nullptr;

	std::string m_title;
	int m_pos_x;
	int m_pos_y;
	int m_width;
	int m_height;
	Uint32 m_flags;

	bool m_vsync = false;
	Uint16 m_max_fps = -1;

	// (Re-)Initialize OpenGL after each context creation
	void init_gl(){

	}
};

}

