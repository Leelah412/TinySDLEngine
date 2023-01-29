#include "ObjLoader.h"

ObjLoader::ObjLoader(){}
ObjLoader::~ObjLoader(){}

std::vector<ObjLoader::Obj> ObjLoader::load(const std::string& path){
	std::vector<Obj> groups;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	// set of used vertex attributes, to avoid creating multiple vertices with same values
	// value is index of existing vertex
	std::unordered_map<glm::ivec3, unsigned int> used_attribs;

	std::ifstream file(path);
	if(!file.is_open()){
		printf("Impossible to open the file !\n");
		return {};
	}
	// Already add a default group
	groups.push_back(Obj());

	std::string line;
	bool has_uvs = false, has_normals = false;
	std::string cur_mat = "";						// material currently applied to a group

	bool success = true;
	while(std::getline(file, line)){
		if(line.length() == 0) continue;

		// Read a vertex attribute
		if(line[0] == 'v'){
			// Position
			if(line[1] == ' '){
				glm::vec3 vertex;
				sscanf_s(line.c_str(), "v %f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				positions.push_back(vertex);
			}
			// UV
			else if(line[1] == 't'){
				has_uvs = true;
				glm::vec2 uv;
				sscanf_s(line.c_str(), "vt %f %f\n", &uv.x, &uv.y);
				uvs.push_back(uv);
			}
			// Normal
			else if(line[1] == 'n'){
				has_normals = true;
				glm::vec3 normal;
				sscanf_s(line.c_str(), "vn %f %f %f\n", &normal.x, &normal.y, &normal.z);
				normals.push_back(normal);
			}
		}
		// Create new group
		else if(line[0] == 'g'){
			// Reset list of used vertices
			// Don't worry, this does not affect the existing vertex attributes we've already read!
			used_attribs = {};

			groups.push_back(Obj());
			groups.back().material = cur_mat;
		}
		// Create face from vertex attributes
		// When we are at "f", assume, that all relevant vertices are already loaded
		else if(line[0] == 'f' && line[1] == ' '){
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
					success = false;
					break;
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
					success = false;
					break;
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
					success = false;
					break;
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
					success = false;
					break;
				}
			}

			std::unordered_map<glm::ivec3, unsigned int>::iterator it;
			// for each vertex of a tri
			for(int i = 0; i < 3; i++){
				// vertex already exists -> point to index of existing vertex
				if((it = used_attribs.find(glm::ivec3(vrts[i][0] - 1, vrts[i][1] - 1, vrts[i][2] - 1))) != used_attribs.end()){
					groups.back().indices.push_back(it->second);
				}
				// otherwise create new vertex
				else{
					ObjVertex vert;
					vert.position = positions.at(vrts[i][0] - 1);
					vert.uv = has_uvs ? uvs.at(vrts[i][1] - 1) : glm::vec2(0.0);
					vert.normal = has_normals ? normals.at(vrts[i][2] - 1) : glm::vec3(0.0);

					groups.back().vertices.push_back(vert);
					groups.back().indices.push_back(groups.back().vertices.size() - 1);
					used_attribs.insert(std::pair<glm::ivec3, unsigned int>(glm::ivec3(vrts[i][0] - 1, vrts[i][1] - 1, vrts[i][2] - 1), groups.back().vertices.size() - 1));
				}
			}
		}
		// Create new group for newly used material
		else if(line.substr(0, 6) == "usemtl"){
			// Reset list of used vertices
			used_attribs = {};
			groups.push_back(Obj());

			char* buf = new char[64];
			sscanf_s(line.c_str(), "usemtl %64s\n", buf, 64);
			cur_mat = buf;
			groups.back().material = cur_mat;
			delete[] buf;
		}
	}

	file.close();

	// If file couldn't be loaded correctly, delete all elements we've created and return an empty set
	if(!success){
		return {};
	}

	// Since we create new groups after encountering "g" or "usemtl", and since a "usemtl" could come right after a "g",
	// there might be empty groups we won't need, so delete them!
	for(int i = groups.size() - 1; i >= 0; i--){
		if(groups.at(i).vertices.empty()){
			groups.erase(groups.begin() + i);
		}
	}

	return groups;
}

ObjLoader::ModelProps ObjLoader::load_model_paths(const std::string& path){
	ModelProps props;
	
	std::ifstream file(path.c_str());
	if(!file.is_open()){
		printf("Impossible to open the file !\n");
		return {};
	}

	JSON data = JSON::parse(file);
	file.close();

	if(!data.contains("type") || data["type"] != "model"){
		std::cout << "ERROR: Couldn't load Model: File not a Model!" << std::endl;
		return props;
	}
	if(!data.contains("body") || !data["body"].is_object()){
		std::cout << "ERROR: Couldn't load Model: Invalid body!" << std::endl;
		return props;
	}
	JSON body = data["body"];

	if(!body.contains("mesh") || !body["mesh"].is_string()){
		std::cout << "ERROR: Couldn't load Model: No or invalid mesh path!" << std::endl;
		return props;
	}
	props.mesh_pth = body["mesh"];

	JSON vms, vm;
	if(!body.contains("vertex_materials") || !(vms = body["vertex_materials"]).is_array()){
		std::cout << "ERROR: Couldn't load Model: 'vertex_material' is invalid!" << std::endl;
		return props;
	}

	for(int i = 0; i < vms.size(); i++){
		if((vm = vms[i]).is_string()){
			props.mat_pths.push_back(vm);
		}
		else{
			std::cout << "WARNING: 'vertex_material' at position " << i << " is invalid! Skipping Material." << std::endl;
			props.mat_pths.push_back("");
		}
	}

	return props;
}

std::vector<ObjLoader::Obj> ObjLoader::load_mesh(const std::string& path){
	std::vector<Obj> objs;

	std::ifstream file(path.c_str());
	if(!file.is_open()){
		printf("Impossible to open the file !\n");
		return {};
	}

	std::string line;
	int mode = -1;			// -1: no mode, 0: vertices, 1: indices

	glm::vec3 v_position;
	glm::vec2 v_uv;
	glm::vec3 v_normal;
	glm::ivec3 indices;

	bool success = true;
	while(std::getline(file, line)){
		// skip empty lines
		if(line.length() == 0){}
		// mode switch
		else if(line.at(0) == '#'){
			if(line.find("vertices") != std::string::npos){
				// add new submesh
				objs.push_back(Obj());
				mode = 0;
			}
			else if(line.find("indices") != std::string::npos){
				mode = 1;
			}
			else if(line.find("material") != std::string::npos){
				char* buf = new char[128];
				sscanf_s(line.c_str(), "#material %128s", buf, 128);
				objs.back().material = buf;
				delete[] buf;
				mode = 2;
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
				success = false;
				break;
			}
			objs.back().vertices.push_back({v_position, v_uv, v_normal});

		}
		// index mode
		else if(mode == 1){
			if(sscanf_s(line.c_str(), "%d %d %d\n", &indices[0], &indices[1], &indices[2]) != 3){
				std::cout << "ERROR: Couldn't load mesh: Invalid .msh file!" << std::endl;
				file.close();
				success = false;
				break;
			}
			objs.back().indices.push_back(indices[0]);
			objs.back().indices.push_back(indices[1]);
			objs.back().indices.push_back(indices[2]);
		}
	}

	file.close();

	// If file couldn't be loaded correctly, delete all elements we've created and return an empty set
	if(!success){
		return {};
	}

	return objs;
}

ObjLoader::Mat* ObjLoader::load_material(const std::string& path){
	std::ifstream file(path.c_str());
	if(!file.is_open()){
		printf("Impossible to open the file !\n");
		return nullptr;
	}
	JSON data = JSON::parse(file);
	file.close();
	return load_material_from_json(data);
}

ObjLoader::Mat* ObjLoader::load_material_from_json(const JSON& data){

	if(!data.contains("type") || data["type"] != "material"){
		std::cout << "ERROR: Couldn't load Material: File not a Material!" << std::endl;
		return nullptr;
	}

	JSON body;
	if(!data.contains("body") || !(body = data["body"]).is_object()){
		std::cout << "ERROR: Couldn't load Material: Invalid body!" << std::endl;
		return nullptr;
	}

	JSON uniforms;
	if(!body.contains("uniforms") || !(uniforms = body["uniforms"]).is_object()){
		std::cout << "ERROR: Couldn't load Material: Invalid body!" << std::endl;
		return nullptr;
	}

	Mat* mat = new Mat();
	JSON mat_type;

	if(uniforms.contains("ambient_color") && (mat_type = uniforms["ambient_color"]).is_object()){
		if(mat_type.contains("r") && mat_type["r"].is_number() &&
			mat_type.contains("g") && mat_type["g"].is_number() &&
			mat_type.contains("b") && mat_type["b"].is_number() &&
			mat_type.contains("a") && mat_type["a"].is_number()){

			mat->ambient_color.r = mat_type["r"];
			mat->ambient_color.g = mat_type["g"];
			mat->ambient_color.b = mat_type["b"];
			mat->ambient_color.a = mat_type["a"];
		}
		else{
			std::cout << "WARNING: 'ambient_color' exists, but is invalid!" << std::endl;
		}
	}

	if(uniforms.contains("diffuse_color") && (mat_type = uniforms["diffuse_color"]).is_object()){
		if(mat_type.contains("r") && mat_type["r"].is_number() &&
			mat_type.contains("g") && mat_type["g"].is_number() &&
			mat_type.contains("b") && mat_type["b"].is_number() &&
			mat_type.contains("a") && mat_type["a"].is_number()){

			mat->diffuse_color.r = mat_type["r"];
			mat->diffuse_color.g = mat_type["g"];
			mat->diffuse_color.b = mat_type["b"];
			mat->diffuse_color.a = mat_type["a"];
		}
		else{
			std::cout << "WARNING: 'diffuse_color' exists, but is invalid!" << std::endl;
		}
	}

	if(uniforms.contains("specular_color") && (mat_type = uniforms["specular_color"]).is_object()){
		if(mat_type.contains("r") && mat_type["r"].is_number() &&
			mat_type.contains("g") && mat_type["g"].is_number() &&
			mat_type.contains("b") && mat_type["b"].is_number() &&
			mat_type.contains("a") && mat_type["a"].is_number()){

			mat->specular_color.r = mat_type["r"];
			mat->specular_color.g = mat_type["g"];
			mat->specular_color.b = mat_type["b"];
			mat->specular_color.a = mat_type["a"];
		}
		else{
			std::cout << "WARNING: 'specular_color' exists, but is invalid!" << std::endl;
		}
	}

	return mat;
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
			if(first_done){
				// create material from uniforms and reset
				mat_files[name_buf] = {
					{"type", "material"},
					{"body", {
						{"name", name_buf},
						{"path", path + "." + name_buf + ".json"},
						{"uniforms", uniforms}
					}}
				};

				uniforms = {};
			}
			else{
				first_done = true;
			}

			sscanf_s(line.c_str(), "newmtl %64s\n", name_buf, 64);
			mat_files.insert(std::pair<std::string, JSON>(name_buf, {}));
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
			{"body", {
				{"name", name_buf},
				{"path", path + "." + name_buf + ".json"},
				{"uniforms", uniforms}
			}}
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

std::map<std::string, JSON> ObjLoader::mtl_from_obj(const std::string& path){

	std::ifstream file(path.c_str());
	if(!file.is_open()){
		printf("Impossible to open the file !\n");
		return {};
	}
	std::string line;

	std::map<std::string, JSON> mats, tmp;
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
				 tmp = mtl_to_material(path.substr(0, i + 1) + buf);
			}
			else{
				tmp = mtl_to_material(buf);
			}

			for(auto& m : tmp){
				mats[m.first] = m.second;
			}
			delete[] buf;
		}
	}

	return mats;
}

std::vector<ObjLoader::Obj> ObjLoader::obj_to_mesh(const std::string& path){
	std::vector<Obj> objs = load(path);

	std::ofstream f_out(std::string(path + ".msh").c_str());
	if(!f_out.is_open()){
		return objs;
	}

	// Create all meshes we've found
	// New mesh always signalled by "#vertices"
	for(int i = 0; i < objs.size(); i++){
		// push vertices in file
		f_out << "#vertices\n";
		for(auto& vs : objs.at(i).vertices){
			f_out << vs.position.x << " " << vs.position.y << " " << vs.position.z << " " << vs.uv.x << " " << vs.uv.y << " " << vs.normal.x << " " << vs.normal.y << " " << vs.normal.z << "\n";
		}
		// push indices in file
		f_out << "\n#indices\n";

		for(size_t j = 0; j < objs.at(i).indices.size(); j += 3){
			f_out << objs.at(i).indices.at(j) << " " << objs.at(i).indices.at(j + 1) << " " << objs.at(i).indices.at(j + 2) << "\n";
		}

		// set material, if it exists
		if(!objs.at(i).material.empty() && (objs.at(i).material != "")){
			f_out << "\n#material " << objs.at(i).material << "\n";
		}

		f_out << "\n";
	}

	f_out.close();
	return objs;
}

void ObjLoader::create_model_from_obj(const std::string& path){
	// Load the mesh
	std::vector<Obj> msh = obj_to_mesh(path);

	// Load the materials
	std::map<std::string, JSON> mat_files = mtl_from_obj(path);

	// Create the materials from the JSONs
	//std::map<std::string, Mat*> mats;
	//for(auto& m : mat_files){
	//	mats.insert( std::pair<std::string, Mat*>(m.first, load_material_from_json(m.second)) );
	//}

	// TODO: make it so, that we don't need to save the path inside the material json
	std::vector<std::string> mat_paths;
	for(int i = 0; i < msh.size(); i++){
		mat_paths.push_back(mat_files[msh.at(i).material]["body"]["path"]);
	}

	JSON data = {
		{"type", "model"},
		{"body", {
			{"mesh", path + ".msh"},
			{"vertex_materials", mat_paths}
		}}
	};

	std::ofstream f_out;
	f_out.open((path + ".model").c_str());

	if(!f_out.is_open()){
		std::cout << "WARNING: Model json file could not be created!" << std::endl;
		return;
	}
	f_out << std::setw(4) << data << std::endl;
	f_out.close();

}
