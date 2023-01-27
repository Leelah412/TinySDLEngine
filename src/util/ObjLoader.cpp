#include "ObjLoader.h"

ObjLoader::ObjLoader(){}
ObjLoader::~ObjLoader(){}

ObjLoader::Obj* ObjLoader::load(const std::string& path){
	Obj* obj = new Obj();

	//std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	// set of used vertex attributes, to avoid creating multiple vertices with same values
	// value is index of existing vertex
	std::unordered_map<glm::ivec3, unsigned int> used_attribs;

	std::ifstream file(path.c_str());
	if(!file.is_open()){
		printf("Impossible to open the file !\n");
		delete obj;
		return nullptr;
	}
	std::string line;

	bool has_uvs = false, has_normals = false;

	while(std::getline(file, line)){
		// Load material library
		if(line.substr(0, 6) == "mtllib"){
			char* buf = new char[64];
			sscanf_s(line.c_str(), "mtllib %64s\n", buf, 64);
			// extract relative path
			auto str_it = path.rbegin();
			int i = path.size() - 1;
			// start from the back, that will be faster
			while(i >= 0){
				// who needs regex lol
				if(path.at(i) == '/') break;
				i--;
			}
			// relative path exists
			if(i != -1){
				mtl_to_material(path.substr(0, i + 1) + buf);
			}
			else{
				mtl_to_material(buf);
			}
			delete[] buf;
		}
		// for this to work, there must be no space before the "v"!
		if(line[0] == 'v'){
			// position
			if(line[1] == ' '){
				glm::vec3 vertex;
				sscanf_s(line.c_str(), "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				positions.push_back(vertex);
			}
			// uv
			else if(line[1] == 't'){

				has_uvs = true;
				glm::vec2 uv;
				sscanf_s(line.c_str(), "vt %f %f\n", &uv.x, &uv.y);
				uvs.push_back(uv);
			}
			// normal
			else if(line[1] == 'n'){
				has_normals = true;
				glm::vec3 normal;
				sscanf_s(line.c_str(), "vn %f %f %f\n", &normal.x, &normal.y, &normal.z);
				normals.push_back(normal);
			}
		}
		// when we are at "f", assume, that all vertices are already loaded
		// create face from vertex attributes
		else if(line[0] == 'f' && line[1] == ' '){
			//std::cout << line << std::endl;
			unsigned int vrts[3][3] = {0};
			int matches;

			if(has_normals && has_uvs){
				matches = sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
					// vertex 1
					&vrts[0][0], &vrts[0][1], &vrts[0][2],
					// vertex 2
					&vrts[1][0], &vrts[1][1], &vrts[1][2],
					// vertex 3
					&vrts[2][0], &vrts[2][1], &vrts[2][2]);

				if(matches != 9){
					printf("File can't be read by our simple parser :( Try exporting with other options\n");
					delete obj;
					return nullptr;
				}
			}
			else if(!has_normals && has_uvs){
				matches = sscanf_s(line.c_str(), "f %d/%d/ %d/%d/ %d/%d/\n",
					// vertex 1
					&vrts[0][0], &vrts[0][1],
					// vertex 2
					&vrts[1][0], &vrts[1][1],
					// vertex 3
					&vrts[2][0], &vrts[2][1]);

				if(matches != 6){
					printf("File can't be read by our simple parser :( Try exporting with other options\n");
					delete obj;
					return nullptr;
				}
			}
			else if(has_normals && !has_uvs){
				matches = sscanf_s(line.c_str(), "f %d//%d %d//%d %d//%d\n",
					// vertex 1
					&vrts[0][0], &vrts[0][2],
					// vertex 2
					&vrts[1][0], &vrts[1][2],
					// vertex 3
					&vrts[2][0], &vrts[2][2]);

				if(matches != 6){
					printf("File can't be read by our simple parser :( Try exporting with other options\n");
					delete obj;
					return nullptr;
				}
			}
			else{
				matches = sscanf_s(line.c_str(), "f %d// %d// %d//\n",
					// vertex 1
					&vrts[0][0],
					// vertex 2
					&vrts[1][0],
					// vertex 3
					&vrts[2][0]);

				if(matches != 3){
					printf("File can't be read by our simple parser :( Try exporting with other options\n");
					delete obj;
					return nullptr;
				}
			}

			std::unordered_map<glm::ivec3, unsigned int>::iterator it;
			// for each vertex of a face
			for(int i = 0; i < 3; i++){
				// vertex already exists -> point to index of existing vertex
				if((it = used_attribs.find(glm::ivec3(vrts[i][0] - 1, vrts[i][1] - 1, vrts[i][2] - 1))) != used_attribs.end()){
					obj->indices.push_back(it->second);
				}
				// otherwise create new vertex
				else{
					ObjVertex vert;
					vert.position = positions.at(vrts[i][0] - 1);
					vert.uv = has_uvs ? uvs.at(vrts[i][1] - 1) : glm::vec2(0.0);
					vert.normal = has_normals ? normals.at(vrts[i][2] - 1) : glm::vec3(0.0);

					obj->vertices.push_back(vert);
					obj->indices.push_back(obj->vertices.size() - 1);
					used_attribs.insert(std::pair<glm::ivec3, unsigned int>(glm::ivec3(vrts[i][0] - 1, vrts[i][1] - 1, vrts[i][2] - 1), obj->vertices.size() - 1));
				}
			}
		}
	}

	file.close();

	return obj;
}

ObjLoader::Obj* ObjLoader::load_mesh(const std::string& path){
	Obj* obj = new Obj();

	std::ifstream file(path.c_str());
	if(!file.is_open()){
		printf("Impossible to open the file !\n");
		delete obj;
		return nullptr;
	}

	std::string line;
	int mode = -1;			// -1: no mode, 0: vertices, 1: indices

	glm::vec3 v_position;
	glm::vec2 v_uv;
	glm::vec3 v_normal;

	glm::ivec3 indices;
	while(std::getline(file, line)){
		// skip empty lines
		if(line.length() == 0){}
		// mode switch
		else if(line.at(0) == '#'){
			if(line.find("vertices") != std::string::npos){
				mode = 0;
			}
			else if(line.find("indices") != std::string::npos){
				mode = 1;
			}
		}
		// vertex mode
		else if(mode == 0){
			if(sscanf_s(line.c_str(), "%f %f %f %f %f %f %f %f\n",
				&v_position[0], &v_position[1], &v_position[2],
				&v_uv[0], &v_uv[1],
				&v_normal[0], &v_normal[1], &v_normal[2]) != 8){

				std::cout << "ERROR: Couldn't load mesh: Invalid .msh file!" << std::endl;
				file.close();
				delete obj;
				return nullptr;
			}
			obj->vertices.push_back({v_position, v_uv, v_normal});

		}
		// index mode
		else if(mode == 1){
			if(sscanf_s(line.c_str(), "%d %d %d\n", &indices[0], &indices[1], &indices[2]) != 3){
				std::cout << "ERROR: Couldn't load mesh: Invalid .msh file!" << std::endl;
				file.close();
				delete obj;
				return nullptr;
			}
			obj->indices.push_back(indices[0]);
			obj->indices.push_back(indices[1]);
			obj->indices.push_back(indices[2]);
		}
	}

	return obj;
	file.close();
}


std::map<std::string, JSON> ObjLoader::mtl_to_material(const std::string& path){

	std::ifstream file(path.c_str());
	if(!file.is_open()){
		printf("Impossible to open the file !\n");
		return {};
	}

	// Since we are loading libraries of materials, we will have to create multiple material files
	std::map<std::string, JSON> mat_files;

	using StringJSON = std::pair<std::string, JSON>;
	std::map<std::string, JSON> uniforms;
	std::string line;

	JSON tmp;
	char* name_buf = new char[64];
	bool first_done = false;				// when landing at "newmtl" for the first time, don't push previous materials, since they don't exist!

	while(std::getline(file, line)){
		// Find the first string 
		size_t cursor = line.find(" ");
		std::string first = line.substr(0, cursor);
		if(first.empty()) continue;

		// New material
		if(first == "newmtl"){
			sscanf_s(line.c_str(), "newmtl %64s\n", name_buf, 64);
			mat_files.insert(std::pair<std::string, JSON>(name_buf, {}));

			if(first_done){
				// create material from uniforms and reset
				mat_files[name_buf] = {
					{"type", "material"},
					{"body", uniforms}
				};

				uniforms = {};
			}
			else{
				first_done = true;
			}
		}
		// Ambient color
		else if(first == "Ka"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Ka %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			tmp = {
					{"r", vec[0]},
					{"g", vec[1]},
					{"b", vec[2]},
					{"a", 1.0f},
			};
			uniforms.insert(StringJSON("ambient_color", tmp));
		}
		// Diffuse color
		else if(first == "Kd"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Kd %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			tmp = {
					{"r", vec[0]},
					{"g", vec[1]},
					{"b", vec[2]},
					{"a", 1.0f},
			};
			uniforms.insert(StringJSON("diffuse_color", tmp));
		}
		// Specular color
		else if(first == "Ks"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Ks %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			tmp = {
					{"r", vec[0]},
					{"g", vec[1]},
					{"b", vec[2]},
					{"a", 1.0f},
			};
			uniforms.insert(StringJSON("specular_color", tmp));
		}
		// Transmission Filter
		//else if(first == "Tf"){
		//	glm::vec3 vec;
		//	sscanf_s(line.c_str(), "Tf %f %f %f\n", &vec[0], &vec[1], &vec[2]);
		//	tmp = {
		//			{"r", vec[0]},
		//			{"g", vec[1]},
		//			{"b", vec[2]},
		//			{"a", 1.0f},
		//	};
		//	uniforms.insert(StringJSON("transmission_filter", tmp));
		//}

	}

	// create last material
	if(!uniforms.empty()){
		// "name_buf" is still name of previous material
		mat_files[name_buf] = {
			{"type", "material"},
			{"body", uniforms}
		};
	}

	delete[] name_buf;
	file.close();

	std::ofstream f_out;
	for(auto& mat : mat_files){
		f_out.open((path + "." + mat.first + ".json").c_str());
		if(!f_out.is_open()){
			std::cout << "WARNING: Material json file could not be created!" << std::endl;
			continue;
		}
		f_out << std::setw(4) << mat.second << std::endl;
		f_out.close();
	}

	return mat_files;

}

ObjLoader::Obj* ObjLoader::obj_to_mesh(const std::string& path){
	Obj* obj = load(path);

	std::ofstream f_out(std::string(path + ".msh").c_str());
	if(!f_out.is_open()){
		return obj;
	}

	// push vertices in file
	f_out << "#vertices\n";
	for(auto& vs : obj->vertices){
		f_out << vs.position.x << " " << vs.position.y << " " << vs.position.z << " " << vs.uv.x << " " << vs.uv.y << " " << vs.normal.x << " " << vs.normal.y << " " << vs.normal.z << "\n";
	}
	// push indices in file
	f_out << "\n#indices\n";

	for(size_t i = 0; i < obj->indices.size(); i += 3){
		f_out << obj->indices.at(i) << " " << obj->indices.at(i + 1) << " " << obj->indices.at(i + 2) << "\n";
	}

	f_out.close();
	return obj;
}

void ObjLoader::create_model_from_obj(const std::string& path){

}
