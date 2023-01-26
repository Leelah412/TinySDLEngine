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

	struct Material{
		JSON shader;
		JSON uniforms;

		//std::string name;
		//glm::vec3 Ka;
		//glm::vec3 Kd;
		//glm::vec3 Ks;
		//glm::vec3 Tf;
		//int	illum;
		////d -halo 0.6600
		//float Ns;
		//int sharpness;
		//float Ni;
	};

	static Obj* load(const std::string& path);
	// Create a default Material json file from the given mtl file and extract the uniforms from the given shader(s)
	static Material* mtl_to_material(const std::string& mtl_path, JSON shader);
	static std::vector<Material*> load_materials(const std::string& path);
	// Create a JSON mesh file from OBJ data
	static void create_mesh_from_obj(const std::string& obj_path, const std::string& msh_name){

	}

private:

};


#endif // !__OBJ_LOADER_H__
