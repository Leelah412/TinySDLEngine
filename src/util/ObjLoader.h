#ifndef __OBJ_LOADER_H__
#define __OBJ_LOADER_H__

#include "util/types.h"
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <json/single_include/nlohmann/json.hpp>

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <fstream>

// Loader for Meshes and Materials extracted from .obj files
class ObjLoader{

public:
	ObjLoader();
	~ObjLoader();

	struct Mat{
		glm::vec4 ambient_color;
		glm::vec4 diffuse_color;
		glm::vec4 specular_color;
	};

	struct ObjVertex{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
	};

	// Representing a mesh group with the given material applied to it, if any
	struct Obj{
		std::vector<ObjVertex> vertices = {};
		std::vector<unsigned int> indices = {};
		std::string material = "";
	};

	struct ModelProps{
		std::string mesh_pth;
		std::vector<std::string> mat_pths;
	};

	// TODO: triangulate non-triangulated faces
	static std::vector<Obj> load(const std::string& path);
	// Get the paths to a Models Mesh and its Materials
	static ModelProps load_model_paths(const std::string& path);
	// Load a mesh (.msh) file in the given path
	static std::vector<Obj> load_mesh(const std::string& path);
	static Mat* load_material(const std::string& path);
	static Mat* load_material_from_json(const JSON& data);
	// Create default Material JSON files from the given mtl library
	// Returns the created JSON data mapped to their filenames
	// All Materials will (currently) have the same uniforms, no matter the shader
	// Currently only supports color values
	static std::map<std::string, JSON> mtl_to_material(const std::string& path);
	// Extract the Materials from the given .obj file and return them
	static std::map<std::string, JSON> mtl_from_obj(const std::string& path);
	// Create a mesh file from OBJ data
	// Returns the vertex and index data
	static std::vector<Obj> obj_to_mesh(const std::string& path);
	// Create a Model JSON from an .obj file, containing Mesh - Material pairs
	static void create_model_from_obj(const std::string& path);

private:

};


#endif // !__OBJ_LOADER_H__
