#ifndef __OBJ_LOADER_H__
#define __OBJ_LOADER_H__

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <json/single_include/nlohmann/json.hpp>
#include <types.h>

#include <string>
#include <vector>
#include <map>
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
	// Load a mesh (.msh) file in the given path
	static Obj* load_mesh(const std::string& path);
	// Create default Material JSON files from the given mtl library
	// Returns the created JSON data mapped to their filenames
	// All Materials will (currently) have the same uniforms, no matter the shader
	// Currently only supports color values
	static std::map<std::string, JSON> mtl_to_material(const std::string& path);
	// Create a mesh file from OBJ data
	// Returns the vertex and index data
	static Obj* obj_to_mesh(const std::string& path);
	// Create a Model JSON from an .obj file, containing Mesh - Material pairs
	static void create_model_from_obj(const std::string& path);

private:

};


#endif // !__OBJ_LOADER_H__
