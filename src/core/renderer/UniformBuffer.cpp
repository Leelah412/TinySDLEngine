#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(GLuint size){
	glGenBuffers(1, &m_buffer_id);
	m_size = 0;
	alloc_buffer(size);
}

UniformBuffer::~UniformBuffer(){
	glDeleteBuffers(1, &m_buffer_id);
}

void UniformBuffer::bind() const{
	glBindBuffer(GL_UNIFORM_BUFFER, m_buffer_id);
}

void UniformBuffer::unbind() const{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

bool UniformBuffer::push(const void* data, GLuint offset, GLuint size){
	// there must be something
	if(!data) return false;
	if(!size) return false;
	// check, if we have enough space
	if(size + offset > m_size) return false;

	// push data
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

	return true;
}

void UniformBuffer::pop(GLuint offset, GLuint size){
	if(!size) return;

	bind();
	// fill given part with zeros
	void* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	std::memset((unsigned char*)ptr + offset, 0, size);
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

bool UniformBuffer::alloc_buffer(GLuint size){
	// Clear previous errors
	while(glGetError() != GL_NO_ERROR);

	bind();
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	
	// Check for success
	if(GLenum error = glGetError()){
		std::cout << "OpenGL Error: " << std::hex << error << std::endl;
		return false;
	}

	m_size = size;
	return true;
}
