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

const VertexBuffer* VertexArray::get_vertex_buffer() const{
	return m_vbo;
}

void VertexArray::create_vertex_buffer(GLuint size, const VertexAttributeLayout& layout, bool is_static){
	// Create VBO with given size (+ size of one vertex for the zero-vertex)
	m_vbo = new VertexBuffer(size + layout.get_stride(), is_static);
	// Assign layout
	m_vbo->set_layout(layout);
	// Set attributes
	set_vertex_buffer(m_vbo);
	// Create zero-vertex
	m_vbo->pop(0, layout.get_stride());
}

void VertexArray::set_vertex_buffer(VertexBuffer* vbo){
	m_vbo = vbo;

	// TODO: binding the vao directly without render manager messes of course with the state management,
	// so maybe use an observer pattern to react to unplanned state changes
	bind();
	m_vbo->bind();

	// Set attributes
	const VertexAttributeLayout& layout = m_vbo->get_layout();
	GLuint stride = layout.get_stride();
	const std::vector<VertexAttribute>& elements = layout.get_elements();
	for(GLuint i = 0; i < elements.size(); i++){
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			i,
			elements.at(i).count,
			elements.at(i).type,
			elements.at(i).normalized ? GL_TRUE : GL_FALSE,
			stride,
			(const void*)elements.at(i).offset
		);
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

}
