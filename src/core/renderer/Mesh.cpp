#include "Mesh.h"

namespace tse{


Mesh::Mesh(){}

Mesh::Mesh(const std::string& path){
	m_filepath = path;
	load_mesh(path);
}

Mesh::Mesh(const void* vertex_data, GLuint vertex_size, const std::vector<GLuint>& indices){
	add_submesh(vertex_data, vertex_size, indices);
}

Mesh::~Mesh(){}

void Mesh::load_mesh(const std::string& path){
	m_filepath = path;
	ObjLoader::Obj* obj = ObjLoader::load(path);
	// remove previous mesh
	delete_mesh();
	// add new one
	float* vdata = new float[obj->vertices.size() * (3 + 2 + 3)];
	for(int i = 0; i < obj->vertices.size(); i++){
		// TODO: check for a smarter method
		// what am i even doing
		ObjLoader::ObjVertex& vert = obj->vertices.at(i);
		// position
		std::memcpy(&vdata[i * 8 + 0], &vert.position.x, 4);
		std::memcpy(&vdata[i * 8 + 1], &vert.position.y, 4);
		std::memcpy(&vdata[i * 8 + 2], &vert.position.z, 4);
		// uv
		std::memcpy(&vdata[i * 8 + 3], &vert.uv.x, 4);
		std::memcpy(&vdata[i * 8 + 4], &vert.uv.y, 4);
		// normal
		std::memcpy(&vdata[i * 8 + 5], &vert.normal.x, 4);
		std::memcpy(&vdata[i * 8 + 6], &vert.normal.y, 4);
		std::memcpy(&vdata[i * 8 + 7], &vert.normal.z, 4);
	}
	// TODO: currently adding only one mesh, enhance this to include multiple submeshes
	add_submesh(vdata, obj->vertices.size() * 8 * sizeof(float), obj->indices);
	// TODO: convert obj to own mesh type

	delete[] vdata;

	m_modified = false;
}

void Mesh::save_mesh(const std::string& path){

}

void Mesh::add_submesh(VertexData* mesh, GLint idx){
	m_submeshes.push_back(mesh);
	m_modified = true;
}

void Mesh::add_submesh(const void* vertex_data, GLuint vertex_size, const std::vector<GLuint>& indices){
	m_submeshes.push_back(new VertexData(vertex_data, vertex_size, indices));
	m_modified = true;
}

VertexData* Mesh::get_submesh(GLuint idx) const{
	if(idx >= m_submeshes.size()) return nullptr;
	return m_submeshes.at(idx);
}

VertexData* Mesh::get_submesh(VertexData* mesh) const{
	auto it = std::find(m_submeshes.begin(), m_submeshes.end(), mesh);
	return (it != m_submeshes.end()) ? *it : nullptr;
}

bool Mesh::has_submesh(VertexData* mesh) const{
	return std::find(m_submeshes.begin(), m_submeshes.end(), mesh) != m_submeshes.end();
}

void Mesh::remove_submesh(GLuint idx){
	if(idx >= m_submeshes.size()) return;
	m_submeshes.erase(m_submeshes.begin() + idx);
}

void Mesh::remove_submesh(VertexData* mesh){
	// TODO: delete, or not delete, that is here the question
	auto it = std::find(m_submeshes.begin(), m_submeshes.end(), mesh);
	if(it == m_submeshes.end()) return;
	m_submeshes.erase(it);
}

const std::vector<VertexData*>& Mesh::get_submesh_list() const{
	return m_submeshes;
}

bool Mesh::delete_mesh(){
	// TODO: delete, or not delete, that is here the question
	for(auto sm : m_submeshes){
		delete sm;
	}
	m_submeshes = {};
	return false;
}

unsigned int Mesh::mesh_vertex_size() const{
	unsigned int count = 0;
	for(auto s : m_submeshes){
		count += s->get_vertex_array_size();
	}
	return count;
}

unsigned int Mesh::mesh_index_count() const{
	unsigned int count = 0;
	for(auto s : m_submeshes){
		count += s->get_indices().size();
	}
	return count;
}

}
