#include "ObjLoader.h"

ObjLoader::ObjLoader()
{
}

ObjLoader::~ObjLoader()
{
}

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

	Mtl* mtl_lib = nullptr;

	while(std::getline(file, line)){
		// Load material library
		if(line.substr(0, 6) == "mtllib"){
			char* buf = new char[64];
			sscanf_s(line.c_str(), "mtllib %s\n", buf);
			mtl_lib = load_material(buf);
			delete[] buf;
		}
		// for this to work, there must be no space before the "v"!
		else if(line[0] == 'v'){
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

	// TODO: put this in an own funtion and remove this from here
	std::ofstream f_out(std::string(path + ".msh").c_str());
	if(!f_out.is_open()) return obj;

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

ObjLoader::Material* ObjLoader::mtl_to_material(const std::string& path, JSON shader){

	// TODO: put this in an own funtion and remove this from here
	std::ofstream f_out(std::string(path + ".json").c_str());
	if(!f_out.is_open()) return nullptr;

	Material* new_mat = new Material();

	std::ifstream file(path.c_str());
	if(!file.is_open()){
		printf("Impossible to open the file !\n");
		return {};
	}
	std::string line;

	std::vector<Mtl*> mats;
	char* name_buf = new char[64];
	while(std::getline(file, line)){
		// Find the first string 
		size_t cursor = line.find(" ");
		std::string first = line.substr(0, cursor);
		if(first.empty()) continue;

		// New material
		if(first == "newmtl"){
			sscanf_s(line.c_str(), "newmtl %s\n", name_buf);
			mats.push_back(new Mtl(name_buf));
		}
		// Ambient color
		else if(first == "Ka"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Ka %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			mats.back()->Ka = vec;
		}
		// Diffuse color
		else if(first == "Kd"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Kd %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			mats.back()->Kd = vec;
		}
		// Specular color
		else if(first == "Ks"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Ks %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			mats.back()->Ks = vec;
		}
		// ???
		else if(first == "Tf"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Tf %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			mats.back()->Tf = vec;
		}
		// Illumination (?)
		else if(first == "illum"){
			sscanf_s(line.c_str(), "illum %d\n", &mats.back()->illum);
		}
		// Sharpness
		else if(first == "sharpness"){
			sscanf_s(line.c_str(), "sharpness %d\n", &mats.back()->sharpness);
		}
		// ???
		else if(first == "Ns"){
			sscanf_s(line.c_str(), "Ns %f\n", &mats.back()->Ns);
		}
		// ???
		else if(first == "Ni"){
			sscanf_s(line.c_str(), "Ni %f\n", &mats.back()->Ni);
		}

	}

	delete[] name_buf;
	file.close();




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

	return nullptr;
}

#if 0
std::vector<ObjLoader::Mtl*> ObjLoader::load_materials(const std::string& path){

	std::ifstream file(path.c_str());
	if(!file.is_open()){
		printf("Impossible to open the file !\n");
		return {};
	}
	std::string line;

	std::vector<Mtl*> mats;
	char* name_buf = new char[64];
	while(std::getline(file, line)){
		// Find the first string 
		size_t cursor = line.find(" ");
		std::string first = line.substr(0, cursor);
		if(first.empty()) continue;

		// New material
		if(first == "newmtl"){
			sscanf_s(line.c_str(), "newmtl %s\n", name_buf);
			mats.push_back(new Mtl(name_buf));
		}
		// Ambient color
		else if(first == "Ka"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Ka %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			mats.back()->Ka = vec;
		}
		// Diffuse color
		else if(first == "Kd"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Kd %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			mats.back()->Kd = vec;
		}
		// Specular color
		else if(first == "Ks"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Ks %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			mats.back()->Ks = vec;
		}
		// ???
		else if(first == "Tf"){
			glm::vec3 vec;
			sscanf_s(line.c_str(), "Tf %f %f %f\n", &vec[0], &vec[1], &vec[2]);
			mats.back()->Tf = vec;
		}
		// Illumination (?)
		else if(first == "illum"){
			sscanf_s(line.c_str(), "illum %d\n", &mats.back()->illum);
		}
		// Sharpness
		else if(first == "sharpness"){
			sscanf_s(line.c_str(), "sharpness %d\n", &mats.back()->sharpness);
		}
		// ???
		else if(first == "Ns"){
			sscanf_s(line.c_str(), "Ns %f\n", &mats.back()->Ns);
		}
		// ???
		else if(first == "Ni"){
			sscanf_s(line.c_str(), "Ni %f\n", &mats.back()->Ni);
		}

		/*
		 Material
		 color and
		 illumination
		 statements:
 				d -halo 0.6600
 
		 Texture
		 map
		 statements:
 				map_Ka -s 1 1 1 -o 0 0 0 -mm 0 1 chrome.mpc
 				map_Kd -s 1 1 1 -o 0 0 0 -mm 0 1 chrome.mpc
 				map_Ks -s 1 1 1 -o 0 0 0 -mm 0 1 chrome.mpc
 				map_Ns -s 1 1 1 -o 0 0 0 -mm 0 1 wisp.mps
 				map_d -s 1 1 1 -o 0 0 0 -mm 0 1 wisp.mps
 				disp -s 1 1 .5 wisp.mps
 				decal -s 1 1 1 -o 0 0 0 -mm 0 1 sand.mps
 				bump -s 1 1 1 -o 0 0 0 -bm 1 sand.mpb
 
		 Reflection
		 map
		 statement:
 				refl -type sphere -mm 0 1 clouds.mpc
		
		
		*/


		// Always add material properties to latest material (i.e. "mats.back()")
		else if(line.substr());
	}

	delete[] name_buf;
	file.close();

	return mats;
}

#endif