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
#define TILE_SIZE 6.0f

using namespace tse;

REGISTER_NODE(WorldGenerator);

class WorldGenerator : public Node{

public:
	struct Floor{
		std::string path;
		unsigned int id;
		unsigned int tile_class;
		unsigned int tile_size_x;
		unsigned int tile_size_y;
	};

	struct PathNode{
		int x;
		int y;
		int radius;
		std::vector<PathNode> neighbors;

		unsigned int distance_to(const PathNode& node){
			int _x = x - node.x;
			int _y = y - node.y;
			return sqrt(_x * _x + _y * _y);
		}

		static bool position_equal(const PathNode& n1, const PathNode& n2){
			return (n1.x == n2.x) && (n1.y == n2.y);
		}

		struct position_less{
			bool operator()(const PathNode& n1, const PathNode& n2) const{
				return (n1.x < n2.x) || ((n1.x == n2.x) && (n1.y <= n2.y));
			}
		};
	};

	WorldGenerator();
	~WorldGenerator();

	void generate();
	std::vector<PathNode> generate_path();
	

	virtual std::string get_class_name() override;

private:

};


#endif // !__WORLD_GENERATOR_H__
