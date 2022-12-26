#include <GameLoop.h>

#include <iostream>
#include <math/Vector.h>

using namespace std;

int main(int argc, char* args[]){

#if not DEBUG
	Vector2 vec = Vector2(0, -123);
	cout << "VECTOR:" << endl;
	cout << "x: " << vec.x << ", y: " << vec.y << endl;
	cout << "Normalized x: " << vec.normalized().x << ", normalized y: " << vec.normalized().y << endl;
	cout << "Normalized length: " << vec.normalized().length() << endl;
	cout << "Angle to origin (rad): " << vec.angle() << endl;
	cout << "Angle to origin (deg): " << vec.angle_deg() << endl;
	cout << "Length: " << vec.length() << endl;
#endif

	GameLoop* game_loop = new GameLoop();

	// TODO: replace with application name
	game_loop->init();
	game_loop->start();
	game_loop->quit();

	delete game_loop;

	return 0;
}