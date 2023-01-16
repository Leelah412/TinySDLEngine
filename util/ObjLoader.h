#ifndef __OBJ_LOADER_H__
#define __OBJ_LOADER_H__

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
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

	static Obj* load(const std::string& path){
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
					sscanf_s(line.c_str(), "vt %f %f %f\n", &normal.x, &normal.y, &normal.z);
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
			//line.find("#shader") != std::string::npos
		}

		file.close();

#if 0
		while(1){



			char lineHeader[128];
			// read the first word of the line
			int res = fscanf_s(file, "%s", lineHeader);
			if(res == EOF)
				break;

			// else : parse lineHeader
			if ( strcmp( lineHeader, "v" ) == 0 ){
				glm::vec3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				positions.push_back(vertex);
			}
			else if(strcmp(lineHeader, "vt") == 0){
				has_uvs = true;
				glm::vec2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				uvs.push_back(uv);
			}

			else if(strcmp(lineHeader, "vn") == 0){
				has_normals = true;
				glm::vec3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				normals.push_back(normal);

			}
			// when we are at "f", assume, that all vertices are already loaded
			// create face from vertex attributes
			else if(strcmp(lineHeader, "f") == 0){
				unsigned int vrts[3][3] = {0};
				int matches;

				if(has_normals && has_uvs){
					matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
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
					matches = fscanf_s(file, "%d/%d/ %d/%d/ %d/%d/\n",
						// vertex 1
						&vrts[0][0], &vrts[0][1], &vrts[0][2],
						// vertex 2
						&vrts[1][0], &vrts[1][1], &vrts[1][2]);

					if(matches != 6){
						printf("File can't be read by our simple parser :( Try exporting with other options\n");
						delete obj;
						return nullptr;
					}
				}
				else if(has_normals && !has_uvs){
					matches = fscanf_s(file, "%d//%d %d//%d %d//%d\n",
						// vertex 1
						&vrts[0][0], &vrts[0][1], &vrts[0][2],
						// vertex 3
						&vrts[2][0], &vrts[2][1], &vrts[2][2]);

					if(matches != 6){
						printf("File can't be read by our simple parser :( Try exporting with other options\n");
						delete obj;
						return nullptr;
					}
				}
				else{
					matches = fscanf_s(file, "%d// %d// %d//\n",
						// vertex 1
						&vrts[0][0], &vrts[0][1], &vrts[0][2]);

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
					if((it = used_attribs.find(glm::ivec3(vrts[i][0], vrts[i][1], vrts[i][2]))) != used_attribs.end()){
						obj->indices.push_back(it->second);
					}
					// otherwise create new vertex
					else{
						ObjVertex vert;
						vert.position = positions.at(vrts[i][0]);
						vert.uv = has_uvs ? uvs.at(vrts[i][1]) : glm::vec2(0.0);
						vert.normal = has_normals ? normals.at(vrts[i][2]) : glm::vec3(0.0);

						obj->vertices.push_back(vert);
						obj->indices.push_back(obj->vertices.size() - 1);
						used_attribs.insert( std::pair<glm::ivec3, unsigned int>(glm::ivec3(vrts[i][0], vrts[i][1], vrts[i][2]), obj->vertices.size() - 1) );
					}
				}
			}

		}
#endif
		return obj;
	}



	static void create_msh_from_obj(const std::string& obj_path, const std::string& msh_name){

	}

};

#endif // !__OBJ_LOADER_H__
