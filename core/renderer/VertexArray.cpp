#include "VertexArray.h"

namespace tse{

VertexArray::VertexArray(){
	glGenVertexArrays(1, &m_buffer_id);
}

VertexArray::~VertexArray(){
	glDeleteVertexArrays(1, &m_buffer_id);
}


/*****************/
/* VERTEX BUFFER */
/*****************/

void VertexArray::bind() const{
	glBindVertexArray(m_buffer_id);
}

void VertexArray::unbind() const{
	glBindVertexArray(0);
}

void VertexArray::bind_shader(Shader* shader, bool rebind){
	// Already bound attr. locs. to shader
	if(!rebind && (m_bound_shaders.find(shader) != m_bound_shaders.end())) return;
	// VBO doesn't exist, nothing to bind
	if(!m_vbo) return;
	// Shader is null, nothing to bind
	if(!shader) return;

	// TODO: binding the vao directly without render manager messes of course with the state management,
	// so maybe use an observer pattern to react to unplanned state changes
	bind();
	m_vbo->bind();

	// Set attributes
	const VertexAttributeLayout& layout = m_vbo->get_layout();
	GLuint stride = layout.get_stride();
	const std::vector<VertexAttribute>& elements = layout.get_elements();
	for(GLuint i = 0; i < elements.size(); i++){
		GLint loc = i;
		// try getting the attribute location by name, otherwise current iteration count is the location
		if(!elements.at(i).name.empty()) loc = glGetAttribLocation(shader->get_program_id(), elements.at(i).name.c_str());
		if(loc == -1) loc = i;
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(
			loc,
			elements.at(i).count,
			elements.at(i).type,
			elements.at(i).normalized ? GL_TRUE : GL_FALSE,
			stride,
			(const void*)elements.at(i).offset
		);
	}

	m_bound_shaders.insert(shader);
}

void VertexArray::unbind_shader(Shader* shader){
	m_bound_shaders.erase(shader);
}

const VertexBuffer* VertexArray::get_vertex_buffer() const{
	return m_vbo;
}

void VertexArray::create_vertex_buffer(GLuint size, const VertexAttributeLayout& layout, bool is_static){
	// Create VBO with given size (+ size of one vertex for the zero-vertex)
	m_vbo = new VertexBuffer(size + layout.get_stride(), is_static);
	// Assign layout
	m_vbo->set_layout(layout);
	// Call "set_vertex_buffer" to set attributes
	set_vertex_buffer(m_vbo);
	// Create zero-vertex
	m_vbo->pop(0, layout.get_stride());
}

void VertexArray::set_vertex_buffer(VertexBuffer* vbo){
	m_vbo = vbo;

	// Rebind VBO attribs. to shaders
	for(auto sh : m_bound_shaders){
		bind_shader(sh, true);
	}
}

bool VertexArray::push_vertex_buffer(const void* data, GLuint offset, GLuint size){
	return m_vbo->push(data, offset, size);
}

void VertexArray::pop_vertex_buffer(GLuint offset, GLuint size){
	m_vbo->pop(offset, size);
}

bool VertexArray::alloc_vertex_buffer(GLuint size, bool is_static){
	if(!m_vbo->alloc_buffer(size + m_vbo->get_layout().get_stride(), is_static)) return false;
	m_vbo->pop(0, m_vbo->get_layout().get_stride());			// zero vertex
}

void VertexArray::set_layout(const VertexAttributeLayout& layout){
	m_vbo->set_layout(layout);
}


/****************/
/* INDEX BUFFER */
/****************/

const IndexBuffer* VertexArray::get_index_buffer() const{
	return m_ibo;
}

void VertexArray::create_index_buffer(GLuint count, bool is_static){
	set_index_buffer(new IndexBuffer(count, is_static));
}

void VertexArray::set_index_buffer(IndexBuffer* ibo){
	bind();
	ibo->bind();
	m_ibo = ibo;
}

bool VertexArray::push_index_buffer(const void* data, GLuint index, GLuint count){
	return m_ibo->push(data, index, count);
}

void VertexArray::pop_index_buffer(GLuint index, GLuint count){
	m_ibo->pop(index, count);
}

bool VertexArray::alloc_index_buffer(GLuint count, bool is_static){
	return m_ibo->alloc_buffer(count, is_static);
}



#ifdef __BUFFER_OBJECT_H__

void VertexArray::update(){
	for(auto obj : m_buffer_entities){
		obj->update();
	}
}

// Render all buffer objects connected to this array
void VertexArray::render() const{
	bind();
	for(BufferEntity* obj : m_buffer_entities){
		obj->render();
	}
}

void VertexArray::add_buffer_entity(BufferEntity* entity, const std::string& name, int index){
	if(!entity) return;
	// dont accept nameless entities
	if(name == "" || name.empty()) return;

	std::map<std::string, unsigned int>::iterator it = m_entity_names.find(name);
	// entity with given name already exists -> do nothing and return
	if(it != m_entity_names.end()){
		std::cout << "WARNING: Trying to add buffer entity with already existing name!" << std::endl;
		return;
	}
	// push entity inbetween at given index
	if((index >= 0) && (index < m_buffer_entities.size())){
		m_buffer_entities.insert(m_buffer_entities.begin() + index, entity);
		// for each entity in the names list coming after entity, increment index by 1
		for(it = m_entity_names.begin(); it != m_entity_names.end(); it++){
			if((*it).second >= index) (*it).second++;
		}
		// add entity name into list
		m_entity_names[name] = index;
	}
	// push entity at the end
	else{
		m_buffer_entities.push_back(entity);
		m_entity_names[name] = m_buffer_entities.size() - 1;
	}

}

void VertexArray::remove_buffer_entity(unsigned int index){
	if(index >= m_buffer_entities.size()) return;

	// remove entity
	m_buffer_entities.erase(begin() + index);
	// remove name
	// VAOs usually not expected to be big or go through many changes for the duration of an application, so O(n) is fine
	// if performance somehow a concern, please us the removal-by-name version, thanks
	for(std::map<std::string, unsigned int>::iterator it = m_entity_names.begin(); it != m_entity_names.end(); it++){
		if((*it).second == index){
			m_entity_names.erase(it);
			break;
		}
	}
}

void VertexArray::remove_buffer_entity(const std::string& name){
	std::map<std::string, unsigned int>::iterator it = m_entity_names.find(name);
	if(it == m_entity_names.end()) return;
	unsigned int idx = (*it).second;
	m_entity_names.erase(it);
	if(idx >= m_buffer_entities.size()){
		std::cout << "WARNING: BufferEntity name exists for VertexArray, but entity itself doesn't!" << std::endl;
		return;
	}
	m_buffer_entities.erase(m_buffer_entities.begin() + idx);
}

void VertexArray::remove_buffer_entity(BufferEntity* entity){
	if(!entity) return;

	unsigned int idx = m_buffer_entities.size();
	for(unsigned int i = 0; i < m_buffer_entities.size(); i++){
		if(m_buffer_entities.at(i) == entity){
			idx = i;
			break;
		}
	}
	
	remove_buffer_entity(idx);
}

const std::vector<BufferEntity*>& VertexArray::get_buffer_entity_list() const{
	return m_buffer_entities;
}
BufferEntity* VertexArray::get_buffer_entity(BufferEntity* entity) const{
	return *std::find(m_buffer_entities.begin(), m_buffer_entities.end(), entity);
}
BufferEntity* VertexArray::get_buffer_entity(unsigned int index) const{
	return m_buffer_entities.at(index);
}

const std::string& VertexArray::get_buffer_entity_name(BufferEntity* entity) const{
	unsigned int idx = m_buffer_entities.size();
	for(unsigned int i = 0; i < m_buffer_entities.size(); i++){
		if(m_buffer_entities.at(i) == entity){
			idx = i;
			break;
		}
	}

	return get_buffer_entity_name(idx);
}

const std::string& VertexArray::get_buffer_entity_name(unsigned int index) const{
	if(index >= m_buffer_entities.size()) return "";

	std::map<std::string, unsigned int>::const_iterator it;
	for(it = m_entity_names.begin(); it != m_entity_names.end(); it++){
		if((*it).second == index){
			return (*it).first;
		}
	}

	return "";
}

BufferEntity* VertexArray::operator[](unsigned int index){
	return m_buffer_entities.at(index);
}

std::vector<BufferEntity*>::iterator VertexArray::begin(){ return m_buffer_entities.begin(); }
std::vector<BufferEntity*>::iterator VertexArray::end(){ return m_buffer_entities.end(); }

const std::map<std::string, unsigned int>& VertexArray::get_entity_names() const{
	return m_entity_names;
}

#endif

}
