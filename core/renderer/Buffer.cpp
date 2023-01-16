#include "Buffer.h"


Buffer::Buffer(){

}
Buffer::~Buffer(){}

/*****************/
/* VERTEX BUFFER */
/*****************/

VertexBuffer::VertexBuffer(GLuint size, bool is_static) : m_size{0}, m_static{is_static}{
	m_layout = VertexAttributeLayout();

	// Initialize buffer
	glGenBuffers(1, &m_buffer_id);
	alloc_buffer(size, is_static);
}
VertexBuffer::~VertexBuffer(){
	glDeleteBuffers(1, &m_buffer_id);
}

void VertexBuffer::bind() const{
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
}
void VertexBuffer::unbind() const{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool VertexBuffer::push(const void* data, GLuint offset, GLuint size){
	// there must be something
	if(!data) return false;
	if(!size) return false;
	// check, if we have enough space
	if(size + offset > m_size) return false;

	// push data
	bind();
	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);

	return true;
}

// TODO: these bind calls are really awful, try to not do that
void VertexBuffer::pop(GLuint offset, GLuint size){
	if(!size) return;

	bind();
	// fill given part with zeros
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	std::memset((unsigned char*)ptr + offset, 0, size);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

GLuint VertexBuffer::get_size() const{
	return m_size;
}

bool VertexBuffer::is_static() const{
	return m_static;
}

bool VertexBuffer::alloc_buffer(GLuint size, bool is_static){
	// Clear previous errors
	while(glGetError() != GL_NO_ERROR);
	bind();
	// TODO: make persistent and coherent
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, is_static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	// Check for success
	if(GLenum error = glGetError()){
		std::cout << "OpenGL Error: " << std::hex << error << std::endl;
		return false;
	}
	m_size = size;
	m_static = is_static;
	return true;
}

const VertexAttributeLayout& VertexBuffer::get_layout() const{
	return m_layout;
}

void VertexBuffer::set_layout(const VertexAttributeLayout& layout){
	m_layout = layout;
}


/****************/
/* INDEX BUFFER */
/****************/

IndexBuffer::IndexBuffer(GLuint max_indices, bool is_static) : m_max_indices{max_indices}, m_static{is_static} {
	glGenBuffers(1, &m_buffer_id);
	alloc_buffer(max_indices, is_static);
}
IndexBuffer::~IndexBuffer(){
	glDeleteBuffers(1, &m_buffer_id);
}

void IndexBuffer::bind() const{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer_id);
}
void IndexBuffer::unbind() const{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool IndexBuffer::push(const void* data, GLuint index, GLuint count){
	// check, if we have enough space
	if(index + count > m_max_indices) return false;
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
	glBufferSubData(GL_ARRAY_BUFFER, index * sizeof(GLuint), count * sizeof(GLuint), data);
	return true;
}

void IndexBuffer::pop(GLuint index, GLuint count){
	glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
	// fill given part with zeros, pointing at the null vertex
	void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	std::memset((unsigned char*)ptr + index * sizeof(GLuint), 0x0, count * sizeof(GLuint));
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

GLuint IndexBuffer::get_max_indices() const{
	return m_max_indices;
}

bool IndexBuffer::is_static() const{
	return m_static;
}

bool IndexBuffer::alloc_buffer(GLuint count, bool is_static){
	// Clear previous errors
	while(glGetError() != GL_NO_ERROR);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffer_id);
	// Create temporary zero array, such that all indices point at empty vertex
	unsigned char* tmp = new unsigned char[count * sizeof(GLuint)];
	if(!tmp){
		std::cout << "ERROR: couldn't allocate space" << std::endl;
		return false;
	}
	std::memset(tmp, 0, count * sizeof(GLuint));
	// Create buffer
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(GLuint), tmp, is_static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
	delete[] tmp;
	// Check for success
	GLenum error;
	if((error = glGetError()) != GL_NO_ERROR){
		std::cout << "OpenGL Error: " << std::hex << error << std::endl;
		return false;
	}
	m_max_indices = count;
	m_static = is_static;
	return true;
}
