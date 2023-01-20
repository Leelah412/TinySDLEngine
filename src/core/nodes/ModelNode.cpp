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


}