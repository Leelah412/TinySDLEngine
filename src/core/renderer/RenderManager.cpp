#include "RenderManager.h"

tse::RenderManager* current_render_manager = new tse::RenderManager();

namespace tse{

RenderManager::RenderManager(){

}

RenderManager::~RenderManager(){

}


/***********/
/* GENERAL */
/***********/

void RenderManager::add_render_operation(RenderOperation* op, int index){
	if(!op) return;
	if(index >= 0 && index < m_render_sequence.size()){
		m_render_sequence.insert(m_render_sequence.begin() + index, op);
		return;
	}
	m_render_sequence.push_back(op);
}

void RenderManager::remove_render_operation(RenderOperation* op, bool delete_op){
	if(!op) return;
	m_render_sequence.erase(std::find(m_render_sequence.begin(), m_render_sequence.end(), op));
	if(delete_op) delete op;
}

void RenderManager::remove_render_operation(unsigned int index, bool delete_op){
	if(index >= m_render_sequence.size()) return;
	RenderOperation* op = m_render_sequence.at(index);
	m_render_sequence.erase(m_render_sequence.begin() + index);
	if(delete_op) delete op;
}

void RenderManager::clear_render_sequence(bool delete_ops){
	if(delete_ops){
		for(size_t i = m_render_sequence.size() - 1; i >= 0; i--){
			delete m_render_sequence.at(i);
		}
	}
	m_render_sequence = {};
}

const std::vector<RenderOperation*>& RenderManager::get_render_sequence(){
	return m_render_sequence;
}

const RenderState& RenderManager::get_state(){
	return m_state;
}


/****************/
/* VERTEX ARRAY */
/****************/

void RenderManager::add_vertex_array(VertexArray* vao, const std::string& name){
	if(name == "" || name.empty()) return;
	if(!vao) return;

	if(m_vertex_arrays.find(name) != m_vertex_arrays.end()){
		std::cout << "WARNING: Trying to overwrite existing VAO." << std::endl;
		return;
	}

	m_vertex_arrays[name] = vao;
}

void RenderManager::remove_vertex_array(const std::string& name, bool delete_vao){
	if(m_vertex_arrays.find(name) == m_vertex_arrays.end()) return;
	if(delete_vao) delete m_vertex_arrays[name];
	m_vertex_arrays.erase(name);
}

VertexArray* RenderManager::get_vertex_array(const std::string& name){
	if(m_vertex_arrays.find(name) == m_vertex_arrays.end()) return nullptr;
	return m_vertex_arrays[name];
}

VertexArray* RenderManager::get_vertex_array_state() const{
	return m_state.current_vertex_array;
}
void RenderManager::set_vertex_array_state(VertexArray* vao){
	m_state.current_vertex_array = vao;
}
void RenderManager::bind(VertexArray* vao){
	if(m_state.current_vertex_array == vao) return;
	if(!vao){
		set_vertex_array_state(nullptr);
		return;
	}
	vao->bind();
	// bind index array too
	vao->get_index_buffer()->bind();
	set_vertex_array_state(vao);
}
void RenderManager::unbind(VertexArray* vao){
	if(m_state.current_vertex_array != vao) return;
	if(!vao) return;
	vao->unbind();
	set_vertex_array_state(nullptr);
}


/***************/
/* FRAMEBUFFER */
/***************/

Framebuffer* RenderManager::get_framebuffer(const std::string& name){
	if(name == "" || name.empty()) return nullptr;
	return m_framebuffers[name];
}

Framebuffer* RenderManager::get_framebuffer_state() const{
	return m_state.current_framebuffer;
}

void RenderManager::set_framebuffer_state(Framebuffer* fbo){
	m_state.current_framebuffer = fbo;
}

void RenderManager::bind(Framebuffer* fbo){
	if(m_state.current_framebuffer == fbo) return;
	if(!fbo){
		// TODO: nullptr -> bind default framebuffer (i.e. window)
		set_framebuffer_state(fbo);
		return;
	}
	fbo->bind();
	set_framebuffer_state(fbo);
}

void RenderManager::unbind(Framebuffer* fbo){
	if(m_state.current_framebuffer != fbo) return;
	if(!fbo) return;
	fbo->unbind();
	set_framebuffer_state(nullptr);
}


/**********/
/* SHADER */
/**********/

void RenderManager::add_shader(Shader* shader, const std::string& name){
	// Can't overwrite default shader
	if(name == TSE_DEFAULT_SHADER_NAME) return;
	if(name == "" || name.empty()) return;
	m_shaders[name] = shader;
}

void RenderManager::remove_shader(const std::string& name){
	// Can't remove default shader
	if(name == TSE_DEFAULT_SHADER_NAME) return;
	m_shaders.erase(name);
}

Shader* RenderManager::get_shader(const std::string& name){
	if(m_shaders.find(name) == m_shaders.end()) return nullptr;
	return m_shaders[name];
}

void RenderManager::create_default_shader(const std::string& path, bool path_is_shader){
	m_shaders[TSE_DEFAULT_SHADER_NAME] = new Shader(path, path_is_shader);
}

void RenderManager::set_default_shader(Shader* shader){
	m_shaders[TSE_DEFAULT_SHADER_NAME] = shader;
}

Shader* RenderManager::get_default_shader(){
	if(m_shaders.find(TSE_DEFAULT_SHADER_NAME) == m_shaders.end()) return nullptr;
	return m_shaders[TSE_DEFAULT_SHADER_NAME];
}

Shader* RenderManager::get_shader_state() const{
	return m_state.current_shader;
}

void RenderManager::set_shader_state(Shader* sh){
	m_state.current_shader = sh;
}

void RenderManager::bind(Shader* sh){
	if(m_state.current_shader == sh) return;
	if(!sh){
		set_shader_state(nullptr);
		return;
	}
	sh->bind();
	set_shader_state(sh);
}

void RenderManager::unbind(Shader* sh){
	if(m_state.current_shader != sh) return;
	if(!sh) return;
	sh->unbind();
	set_shader_state(nullptr);
}


/**********/
/* CAMERA */
/**********/

const std::set<Camera*>& RenderManager::get_active_cameras(){
	return m_active_cameras;
}

void RenderManager::activate_camera(Camera* camera){
	m_active_cameras.insert(camera);
}

void RenderManager::deactivate_camera(Camera* camera){
	m_active_cameras.erase(camera);
}


/*********/
/* LIGHT */
/*********/

const std::set<Light*>& RenderManager::get_active_lights(){
	return m_lights;
}
void RenderManager::activate_light(Light* light){
	m_lights.insert(light);
}
void RenderManager::deactivate_light(Light* light){
	m_lights.erase(light);
}


}