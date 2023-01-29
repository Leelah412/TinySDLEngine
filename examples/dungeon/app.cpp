#include "app.h"

TSE_USE_APP(Dungeon, "example/dungeon/config.json")

Dungeon::Dungeon(){}
Dungeon::~Dungeon(){}

void Dungeon::handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext){
	if(event == SEM_QUIT) stop();
	if((event == SEM_KEY_PRESSED) && sdl_event.key.keysym.sym == SDLK_ESCAPE) stop();
}

int Dungeon::init(){
	listen_event(SEM_KEY_PRESSED, SEM_EVENT_EXT(SDLK_ESCAPE));

	SDL_SetRelativeMouseMode(SDL_TRUE);
	return 0;
}
