#include "ModelNode.h"

namespace tse{

ModelNode::ModelNode(){

}

ModelNode::ModelNode(const std::string& path, bool unique){
	load_model(path, unique);
}
ModelNode::ModelNode(Model* model){
	set_model(model);
}

ModelNode::~ModelNode(){
	delete_model();
}

void ModelNode::update(const time_t& delta){
	// push/update mesh in(to) buffer
	
}

bool ModelNode::load_model(const std::string& path, bool unique){
	// delete previous model
	delete_model();
	// create new model
	m_model = new Model();
	if(!m_model->load_mesh(path)) return false;
	// add model to render list
	if(m_visible)
		IModelManager->add_model(m_model);
	return true;
}

bool ModelNode::set_model(Model* model){
	if(m_model == model) return false;
	delete_model();
	m_model = model;
	if(m_visible)
		IModelManager->add_model(m_model);
	return true;
}

void ModelNode::delete_model(){
	IModelManager->remove_model(m_model);
	delete m_model;
	m_model = nullptr;
}

Model* ModelNode::get_model() const{
	return m_model;
}

bool ModelNode::is_visible() const{
	return m_visible;
}
void ModelNode::set_visibility(bool visible){
	m_visible = visible;
	// TODO push to/ remove from list of models to be rendered, depending on new visibility
	if(m_model){
		visible ? IModelManager->add_model(m_model) : IModelManager->remove_model(m_model);
	}
}

JSON ModelNode::save(){
	JSON data = Node::save();
	if(is_exempt_from_saving()) return data;

	data["visible"] = m_visible;

	Model* mod = m_model;
	Mesh* mesh = mod->get_mesh();

	// Model doesn't exist, has no mesh or is not created from an existing file
	if(!m_model || !mesh || mesh->get_filepath().empty() || (mesh->get_filepath() == "")) return data;

	// We don't care, if the Mesh of a Model is dynamically changed or not,
	// just save the path to the Models mesh and the VertexData - Material combinations

	const std::unordered_map<VertexData*, VertexMaterial>& vms = mod->get_vertex_material_map();
	const std::vector<VertexData*>& subs = mesh->get_submesh_list();

	JSON vms_json = JSON::array();

	std::unordered_map<VertexData*, VertexMaterial>::const_iterator vms_it;
	for(unsigned int i = 0; i < subs.size(); i++){
		if( (vms_it = vms.find(subs.at(i))) != vms.end() && vms_it->second.material){
			std::string str = vms_it->second.material->get_filepath();
			// Add Material to array, if filepath exists...
			if(!str.empty() && (str != "")){
				vms_json.push_back({"material",  str});
			}
			// ... otherwise add whole material data
			// TODO: maybe lets do that another time...
			else{
				auto sh = vms_it->second.material->get_shader();
				JSON mat = {
						{"material", ""},
						{"shader", (sh ? sh->get_filepath() : "null")}
					};

				vms_json.push_back(mat);
			}
		}
		else{
			// Combine with empty Material
			vms_json.push_back({});
		}
	}

	JSON model;
	model = {
		{"mesh", mesh->get_filepath()},
		{"vertex_material", vms_json}
	};

	data["model"] = model;

	return data;
}

void ModelNode::load(const JSON& data){

	if(data.contains("visible") && data["visible"].is_boolean()){
		m_visible = data["visible"];
	}
	else{
		m_visible = true;
		std::cout << "WARNING: Loading 'ModelNode': 'visible' doesn't exist or is not a boolean!" << std::endl;
	}

	if(!data.contains("model") || !data["model"].is_object()){
		set_model(new Model());
		std::cout << "WARNING: Loading 'ModelNode': 'model' doesn't exist or is not an object!" << std::endl;
		return;
	}
	JSON model = data["model"];

	if(!model.contains("mesh") || !model["mesh"].is_string() || model["mesh"].empty() || (model["mesh"] == "")){
		set_model(new Model());
		std::cout << "WARNING: Loading 'ModelNode': 'model.mesh' doesn't exist or is not a string or is an empty string!" << std::endl;
		return;
	}

	Mesh* mesh = (Mesh*)ResManager->load_and_copy_resource(model["mesh"], RES_TYPE::MESH);
	Model* cl_model = new Model(mesh);

	JSON vms;
	if(!model.contains("vertex_material") || !(vms = model["vertex_material"]).is_array()){
		set_model(cl_model);
		std::cout << "WARNING: Loading 'ModelNode': 'model.vertex_material' doesn't exist or is not an array!" << std::endl;
		return;
	}
	
	Material* cur_mat = nullptr;
	VertexData* vd = nullptr;
	int i = 0;
	for(auto& vm : vms){
		// TODO: Load material from path
		if(vm.contains("material") && vm["material"].is_string() && !vm["material"].empty() && (vm["material"] != "")){
			cur_mat = new Material();
			vd = mesh->get_submesh(i);
			cl_model->assign_material(vd, cur_mat);

			i++;
			continue;
		}

		// Load material from data
		// First get the shader path to create the material from the shader
		if(vm.contains("shader") && vm["shader"].is_string() && !vm["shader"].empty() && (vm["shader"] != "")){
			// Load Shader with ResourceManager
			Shader* sh = (Shader*)ResManager->load_resource(vm["shader"], RES_TYPE::SHADER);
			cur_mat = new Material(sh);
		}

		// Assign material to the current submesh
		vd = mesh->get_submesh(i);
		cl_model->assign_material(vd, cur_mat);

		i++;
	}

	// Must set model AFTER loading all VMs (currently)
	set_model(cl_model);
}

std::string ModelNode::get_class_name(){
	return NodeName(ModelNode);
}


}