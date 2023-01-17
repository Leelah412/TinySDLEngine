#include "Model.h"

namespace tse{

unsigned int Model::m_model_id_count = 0;

Model::Model() : m_mesh{nullptr}{
	m_model_id = ++m_model_id_count;
}

Model::Model(const std::string& path, bool unique){
	load_mesh(path, unique);
}

Model::Model(Mesh* mesh, bool unique){
	set_mesh(mesh, unique);
}

Model::~Model(){}

bool Model::load_mesh(const std::string& path, bool unique){
	reset_mesh();

	if(unique){
		m_mesh = (Mesh*)ResManager->load_and_copy_resource(path, RES_TYPE::MESH);
	}
	else{
		m_mesh = (Mesh*)ResManager->load_resource(path, RES_TYPE::MESH);
	}
	if(!m_mesh) return false;

	init_vertex_material_list();
	return true;
}

bool Model::set_mesh(Mesh* mesh, bool unique){
	if(!mesh) return false;
	// mesh is managed by resource manager
	if(ResManager->resource_loaded(mesh->get_filepath(), mesh) && load_mesh(mesh->get_filepath(), unique))
		return true;
	// mesh is independent or couldn't be loaded with resource manager
	reset_mesh();
	m_mesh = mesh;
	init_vertex_material_list();
	return true;
}

void Model::reset_mesh(){
	ResManager->unload_resource(m_mesh);
	m_mesh = nullptr;
	m_vertex_materials = {};
}

const Mesh* Model::get_mesh(){
	return m_mesh;
}

const std::unordered_map<VertexData*, VertexMaterial>& Model::get_vertex_material_map(){
	return m_vertex_materials;
}

const std::vector<Ref<VertexMaterial>> Model::get_vertex_materials(){
	std::vector<Ref<VertexMaterial>> vals;
	for(auto& vm : m_vertex_materials){
		vals.push_back(std::ref(vm.second));
	}
	return vals;
}

void Model::add_vertex_material(VertexData* v, Material* m, std::vector<Uniform> uniform_changes){
	// check, if vertex data belongs to mesh
	if(!m_mesh->has_submesh(v)) return;
	m_vertex_materials.insert(std::pair<VertexData*, VertexMaterial>(v, VertexMaterial(v, m, uniform_changes)));
}

void Model::assign_material(VertexData* v, Material* m, std::vector<Uniform> uniform_changes){
	if(!m_mesh->has_submesh(v)) return;
	auto vm = m_vertex_materials.find(v);
	if(vm == m_vertex_materials.end()) return;

	vm->second.vertex_data = v;
	vm->second.material = m;
	vm->second.uniform_changes = uniform_changes;
}

void Model::remove_material(VertexData* v){
	auto vm = m_vertex_materials.find(v);
	if(vm == m_vertex_materials.end()) return;

	vm->second.vertex_data = v;
	vm->second.material = nullptr;
	vm->second.uniform_changes = {};
}

unsigned int Model::get_model_id(){
	return m_model_id;
}

void Model::init_vertex_material_list(){
	for(VertexData* mesh : m_mesh->get_submesh_list()){
		// TODO: consider passing materials per VD, when loading from file, or use default shader
		VertexMaterial vm(mesh, nullptr, {});
		m_vertex_materials.insert(std::pair<VertexData*, VertexMaterial>(mesh, vm));
	}
}

}