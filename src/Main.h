#ifndef __MAIN_H__
#define __MAIN_H__

#include <Application.h>

#include "nodes/FirstPersonCamera.h"
#include "nodes/ModelNode.h"
#include "nodes/LightNode.h"
#include "util/ObjLoader.h"

extern tse::Application* tse::create_application();

int main(int argc, char* args[]){
	tse::Application* app = tse::create_application();

	app->run();
	delete app;

	return 0;
}

#endif // !__MAIN_H__