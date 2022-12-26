#include "Mesh.h"

Mesh::Mesh(){}

Mesh::~Mesh(){
	m_mesh_object = MeshObject();
}

Mesh::Mesh(MeshObject mesh) : m_mesh_object{mesh}{}

void Mesh::bind() const{

}

void Mesh::unbind() const{

}

void Mesh::load_texture(const std::string& path, bool unique){
	if(unique){
		m_texture = new Texture(path);
		return;
	}
	m_texture = (Texture*) ResManager->load_resource(path, RES_TYPE::TEXTURE);
}

void Mesh::load_normal(const std::string& path, bool unique){
	if(unique){
		m_normal = new Texture(path);
		return;
	}
	m_normal = (Texture*)ResManager->load_resource(path, RES_TYPE::TEXTURE);
}

void Mesh::make_texture_unique(){
	// resource already unique
	if(!ResManager->resource_loaded(m_texture->get_filepath(), m_texture)) return;
	m_normal = new Texture(m_texture->get_filepath());
}

void Mesh::make_normal_unique(){
	// resource already unique
	if(!ResManager->resource_loaded(m_normal->get_filepath(), m_normal)) return;
	m_normal = new Texture(m_texture->get_filepath());
}
