#include <GameLoop.h>

#include <iostream>
#include <math/Vector.h>

using namespace std;

int main(int argc, char* args[]){

	GameLoop* game_loop = new GameLoop();

	// TODO: replace with application name
	game_loop->init();
	game_loop->start();
	game_loop->quit();

	delete game_loop;

	return 0;
}