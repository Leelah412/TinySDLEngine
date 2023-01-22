#ifndef __OBJ_LOADER_H__
#define __OBJ_LOADER_H__

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <json/single_include/nlohmann/json.hpp>
#include <types.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>

// WARNING: bad, unoptimized and half-stolen code to make sure things work first

class ObjLoader{

public:
	ObjLoader();
	~ObjLoader();

	struct ObjVertex{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	struct Obj{
		std::vector<ObjVertex> vertices = {};
		std::vector<unsigned int> indices = {};
	};

	static Obj* load(const std::string& path);
	// Create a JSON mesh file from OBJ data
	static void create_mesh_from_obj(const std::string& obj_path, const std::string& msh_name){

	}

};

#endif // !__OBJ_LOADER_H__
