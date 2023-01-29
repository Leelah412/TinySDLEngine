#include "WorldGenerator.h"

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
	unsigned int id;
	std::vector<Floor> lst;
	
	// load paths
	while(std::getline(floor, line)){
		sscanf_s(line.c_str(), "%64s %d\n", buf, 64, &id);
		lst.push_back(Floor({std::string(MESH_PATH) + "/floor/" + buf + ".obj", id}));
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
	unsigned int tiles = lst.size();
	std::srand(std::time(nullptr));
	ModelNode* cur_model;
	float x, y, z;
	for(int i = 0; i < WORLD_WIDTH; i++){
		for(int j = 0; j < WORLD_HEIGHT; j++){
			cur_model = new ModelNode(lst.at(std::rand() % tiles).path);
			x = i * TILE_SIZE - (TILE_SIZE * WORLD_WIDTH / 2.0f);
			y = 0;
			z = j * TILE_SIZE - (TILE_SIZE * WORLD_HEIGHT / 2.0f);
			cur_model->set_position(x, y, z);
			add_child(cur_model);
		}
	}

}
