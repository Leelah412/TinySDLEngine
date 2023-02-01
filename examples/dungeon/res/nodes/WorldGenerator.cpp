#include "WorldGenerator.h"
#include <random>

WorldGenerator::WorldGenerator(){}
WorldGenerator::~WorldGenerator(){}

void WorldGenerator::generate(){
	// load objects saved in text file
	std::string path(std::string(MESH_PATH) + "/floor/objects.txt");
	std::ifstream floor(path);
	if(!floor.is_open()){
		std::cout << "ERROR: Couldn't open '" << path << "'. Abort." << std::endl;
		return;
	}

	std::string line;
	// first two lines are not read
	std::getline(floor, line);
	std::getline(floor, line);
	char* buf = new char[64];
	unsigned int id, tile_class, tile_size_x, tile_size_y;
	std::vector<Floor> lst;
	
	// floor types
	const unsigned int C_STAIRS = 1, C_FLOOR_1 = 2, C_FLOOR_2 = 3, C_SPIKES = 4;

	// load paths
	while(std::getline(floor, line)){
		sscanf_s(line.c_str(), "%64s %d %d %d %d\n", buf, 64, &id, &tile_class, &tile_size_x, &tile_size_y);
		lst.push_back(Floor({std::string(MESH_PATH) + "/floor/" + buf + ".obj", id, tile_class, tile_size_x, tile_size_y}));
	}

	delete[] buf;
	floor.close();

	struct stat buffer;
	// load objects from loaded paths
	for(int i = 0; i < lst.size(); i++){
		// only load, if model doesnt already exist
		if(stat((lst.at(i).path + ".model").c_str(), &buffer))
			ObjLoader::create_model_from_obj(lst.at(i).path);
		// after creating 
		lst.at(i).path += ".model";
	}

	// randomly generate tile models
	std::vector<PathNode> path_nodes = generate_path();

	// left/up shift position by half of max size

	//unsigned int tiles = lst.size();
	//std::srand(std::time(nullptr));
	//ModelNode* cur_model;
	//float x, y, z;
	//for(int i = 0; i < WORLD_WIDTH; i++){
	//	for(int j = 0; j < WORLD_HEIGHT; j++){
	//		cur_model = new ModelNode(lst.at(std::rand() % tiles).path);
	//		x = i * TILE_SIZE - (TILE_SIZE * WORLD_WIDTH / 2.0f);
	//		y = 0;
	//		z = j * TILE_SIZE - (TILE_SIZE * WORLD_HEIGHT / 2.0f);
	//		cur_model->set_position(x, y, z);
	//		add_child(cur_model);
	//	}
	//}
}

std::vector<WorldGenerator::PathNode> WorldGenerator::generate_path(){
	std::vector<PathNode> nodes;

	std::random_device rd;
	std::mt19937 gen(rd());
	// create a random amount of path points in the given range
	std::uniform_int_distribution<> distr(4, max(4, WORLD_WIDTH * WORLD_HEIGHT / 16));
	unsigned int point_count = distr(gen);

	// min. distance between each point
	// TODO: check, if its even possible with generated point count, but if we keep the numbers large enough for now, this should be fine
	//const float min_dist = sqrt(2.1);		// sqrt of 2.1 to exclude "diagonal" neighbors too
	// save the points to be created
	std::set<PathNode, PathNode::position_less> created;
	// randomly choose a few points inside the given range
	std::uniform_int_distribution<> size_x(0, WORLD_WIDTH - 1), size_y(0, WORLD_HEIGHT - 1);
	PathNode cur_pn, start_pn;
	bool done = false;
	for(int i = 0; i < point_count && !done; i++){
		PathNode cur_pn = {size_x(gen), size_y(gen), 1};
		start_pn = cur_pn;

		// shift position by one, until we hit a spot we can inherit, or until we are back at the starting point
		// in the latter case, break the for loop and continue with the next part with the points we've created
		while(created.find(cur_pn) != created.end()){
			if(cur_pn.x < WORLD_WIDTH - 1) cur_pn.x++;
			else if(cur_pn.y < WORLD_HEIGHT - 1){
				cur_pn.y++;
				cur_pn.x = 0;
			}
			else{
				cur_pn.x = 0;
				cur_pn.y = 0;
			}

			if(PathNode::position_equal(cur_pn, start_pn)){
				done = true;
				break;
			}
		}

		if(!done) created.insert(cur_pn);
	}

	// create paths


	return nodes;
}

std::string WorldGenerator::get_class_name(){
	return NodeName(WorldGenerator);
}
