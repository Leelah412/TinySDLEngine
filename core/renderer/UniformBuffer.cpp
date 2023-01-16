#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(GLuint size, GLuint binding){
	glGenBuffers(1, &m_buffer_id);
	alloc_buffer(size, binding);
}

UniformBuffer::~UniformBuffer(){
	glDeleteBuffers(1, &m_buffer_id);
}

void UniformBuffer::bind() const{
	glBindBuffer(GL_UNIFORM_BUFFER, m_buffer_id);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_binding, m_buffer_id);
}

void UniformBuffer::unbind() const{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, m_binding, 0);
}

void UniformBuffer::rebind(GLuint binding){
	m_binding = binding;
	bind();
}

bool UniformBuffer::alloc_buffer(GLuint size, GLuint binding){
	bind();
	// TODO: check again, if its correct with the static draw thing
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	unbind();
}
