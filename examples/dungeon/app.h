#ifndef __APP_H__
#define __APP_H__

#include <Application.h>

using namespace tse;

class Dungeon : public Application{

public:
	Dungeon();
	virtual ~Dungeon();

	void handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext) override;
	int init() override;
	int quit() override;

private:

};

#endif // !__APP_H__
