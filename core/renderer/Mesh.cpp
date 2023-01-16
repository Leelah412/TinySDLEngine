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
	// TODO: convert obj to own mesh type

}

void Mesh::add_submesh(VertexData* mesh){
	m_submeshes.insert(mesh);
}

void Mesh::add_submesh(const void* vertex_data, GLuint vertex_size, const std::vector<GLuint>& indices){
	m_submeshes.insert(new VertexData(vertex_data, vertex_size, indices));
}

const VertexData* Mesh::get_submesh(VertexData* mesh) const{
	auto it = m_submeshes.find(mesh);
	return (it != m_submeshes.end()) ? *it : nullptr;
}

bool Mesh::has_submesh(VertexData* mesh) const{
	return m_submeshes.find(mesh) != m_submeshes.end();
}

void Mesh::remove_submesh(VertexData* mesh){
	// TODO: delete, or not delete, that is here the question
	m_submeshes.erase(mesh);
}

const std::set<VertexData*>& Mesh::get_submesh_list() const{
	return m_submeshes;
}

bool Mesh::delete_mesh(){
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
