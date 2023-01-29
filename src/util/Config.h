#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "util/types.h"
#include <SDL.h>
#include <json/single_include/nlohmann/json.hpp>
#include <string>
#include <iostream>

#define TSE_CONFIG_DEFAULT_TITLE "TinySDLEngine"
#define TSE_CONFIG_DEFAULT_WINDOW_X SDL_WINDOWPOS_UNDEFINED
#define TSE_CONFIG_DEFAULT_WINDOW_Y SDL_WINDOWPOS_UNDEFINED
#define TSE_CONFIG_DEFAULT_WINDOW_W 1280
#define TSE_CONFIG_DEFAULT_WINDOW_H 720
#define TSE_CONFIG_DEFAULT_MAX_FPS 60
#define TSE_CONFIG_DEFAULT_VSYNC_ON true

namespace tse{

// Static class containing program configurations
typedef struct Config{
	static std::string title;
	static int window_x;
	static int window_y;
	static int window_w;
	static int window_h;
	static int max_fps;

	/* GL Config */
	static bool vsync_on;
};

// Applies the configuration from the config file, or default, if it's invalid
int apply_config(const JSON& config);

}


#endif // !__CONFIG_H__
