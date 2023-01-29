#include "Config.h"

namespace tse{

std::string Config::title = "TinySDLEngine";
int Config::window_x = SDL_WINDOWPOS_UNDEFINED;
int Config::window_y = SDL_WINDOWPOS_UNDEFINED;
int Config::window_w = 1280;
int Config::window_h = 720;
int Config::max_fps = 60;
bool Config::vsync_on = true;

int apply_config(const JSON& config){

	if(config["type"] != "config"){
		std::cout << "ERROR: File is not a config file! Abort." << std::endl;
		return -1;
	}

	JSON body;
	if(!config.contains("body") || !(body = config["body"]).is_object()){
		std::cout << "ERROR: File is not a config file! Abort." << std::endl;
		return -1;
	}

	// If title doesn't exist or is invalid, assign default one
	if(!body.contains("title") || !body["title"].is_string())
		Config::title = TSE_CONFIG_DEFAULT_TITLE;
	else
		Config::title = body["title"];

	// Window
	if(!body.contains("window_x") || !body["window_x"].is_number_integer())
		Config::window_x = TSE_CONFIG_DEFAULT_WINDOW_X;
	else
		Config::window_x = body["window_x"];

	if(!body.contains("window_y") || !body["window_y"].is_number_integer())
		Config::window_y = TSE_CONFIG_DEFAULT_WINDOW_Y;
	else
		Config::window_y = body["window_y"];

	if(!body.contains("window_w") || !body["window_w"].is_number_integer())
		Config::window_w = TSE_CONFIG_DEFAULT_WINDOW_W;
	else
		Config::window_w = body["window_w"];

	if(!body.contains("window_h") || !body["window_h"].is_number_integer())
		Config::window_h = TSE_CONFIG_DEFAULT_WINDOW_H;
	else
		Config::window_h = body["window_h"];

	if(!body.contains("max_fps") || !body["max_fps"].is_number_integer())
		Config::max_fps = TSE_CONFIG_DEFAULT_MAX_FPS;
	else
		Config::max_fps = body["max_fps"];

	// GL Config
	JSON gl;
	if(!body.contains("gl_config") || !(gl = body["gl_config"]).is_object()){
		Config::vsync_on = TSE_CONFIG_DEFAULT_VSYNC_ON;
	}
	else{
		if(!gl.contains("vsync_on") || !gl["vsync_on"].is_number_integer())
			Config::vsync_on = TSE_CONFIG_DEFAULT_VSYNC_ON;
		else
			Config::vsync_on = gl["vsync_on"];
	}

	return 0;
}

}