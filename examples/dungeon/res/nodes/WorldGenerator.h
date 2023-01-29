#ifndef __WORLD_GENERATOR_H__
#define __WORLD_GENERATOR_H__

#include "nodes/ModelNode.h"
#include "util/ObjLoader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <ctime>

#define MESH_PATH "examples/dungeon/res/mesh"
#define WORLD_WIDTH 16
#define WORLD_HEIGHT 16			// "horizontal" height!
#define TILE_SIZE 8.0f

using namespace tse;

REGISTER_NODE(WorldGenerator);

class WorldGenerator : public Node{

public:
	struct Floor{
		std::string path;
		unsigned int id;
	};


	WorldGenerator();
	~WorldGenerator();

	void generate();


private:

};


#endif // !__WORLD_GENERATOR_H__
