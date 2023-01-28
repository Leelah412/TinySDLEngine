#include "Model.h"

namespace tse{

unsigned int Model::m_model_id_count = 0;

Model::Model() : m_mesh{nullptr}{
	m_model_id = ++m_model_id_count;
}

Model::Model(const std::string& path, bool unique){
	m_model_id = ++m_model_id_count;
	load_model(path, unique);
}

Model::Model(Mesh* mesh, bool unique){
	m_model_id = ++m_model_id_count;
	set_mesh(mesh, unique);
}

Model::~Model(){}

bool Model::load_model(const std::string& path, bool unique){
	ObjLoader::ModelProps pths = ObjLoader::load_model_paths(path);
	if(!load_mesh(pths.mesh_pth)){
		std::cout << "ERROR: Couldn't load Model with Mesh at '" << pths.mesh_pth << "'. Abort." << std::endl;
		return false;
	}

	const std::vector<VertexData*>& sl = m_mesh->get_submesh_list();
	// we can only assign materials to the vertex data, that exist
	// in normal cases tho, values should always be equivalent!
	int size = std::min(sl.size(), pths.mat_pths.size());
	for(int i = 0; i < size; i++){
		assign_material(sl.at(i), (Material*)ResManager->load_resource(pths.mat_pths.at(i), RES_TYPE::MATERIAL));
	}
	return true;
}

bool Model::load_mesh(const std::string& path, bool unique){
	reset_mesh();

	if(unique){
		m_mesh = (Mesh*)ResManager->load_and_copy_resource(path, RES_TYPE::MESH);
	}
	else{
		m_mesh = (Mesh*)ResManager->load_resource(path, RES_TYPE::MESH);
	}
	if(!m_mesh) return false;
	//m_unique = unique;

	init_vertex_material_list();
	return true;
}

bool Model::set_mesh(Mesh* mesh, bool unique){
	if(!mesh) return false;
	//m_unique = unique;
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
	//m_unique = false;
	m_mesh = nullptr;
	m_vertex_materials = {};
}

Mesh* Model::get_mesh() const{
	return m_mesh;
}

const std::unordered_map<VertexData*, VertexMaterial>& Model::get_vertex_material_map() const{
	return m_vertex_materials;
}

std::vector<Ref<VertexMaterial>> Model::get_vertex_materials(){
	std::vector<Ref<VertexMaterial>> vals;
	for(auto& vm : m_vertex_materials){
		vals.push_back(std::ref(vm.second));
	}
	return vals;
}

void Model::add_vertex_material(VertexData* v, Material* m, std::vector<Uniform> uniform_changes){
	// check, if vertex data belongs to mesh
	if(!m_mesh->has_submesh(v)) return;
	VertexMaterial mat(v, m, uniform_changes);
	// don't forget to assign ID to access Model containing VertexMaterial faster, when rendering in ModelManager!
	mat.id = m_model_id;
	m_vertex_materials.insert(std::pair<VertexData*, VertexMaterial>(v, mat));
}

void Model::assign_material(VertexData* v, Material* m, std::vector<Uniform> uniform_changes){
	if(!m_mesh->has_submesh(v)) return;
	auto vm = m_vertex_materials.find(v);
	if(vm == m_vertex_materials.end()) return;

	vm->second.id = m_model_id;						// prob. redundant, but to be safe
	vm->second.vertex_data = v;
	vm->second.material = m;
	vm->second.uniform_changes = uniform_changes;
}

void Model::remove_material(VertexData* v){
	auto vm = m_vertex_materials.find(v);
	if(vm == m_vertex_materials.end()) return;

	vm->second.id = m_model_id;
	vm->second.vertex_data = v;
	vm->second.material = nullptr;
	vm->second.uniform_changes = {};
}

const glm::mat4& Model::get_transform(){
	return m_transform;
}

void Model::set_transform(const glm::mat4& transform){
	m_transform = transform;
}

unsigned int Model::get_model_id() const{
	return m_model_id;
}

void Model::init_vertex_material_list(){
	for(VertexData* mesh : m_mesh->get_submesh_list()){
		add_vertex_material(mesh, nullptr, {});
	}
}

}