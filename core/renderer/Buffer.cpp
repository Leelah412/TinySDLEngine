#include "Buffer.h"


Buffer::Buffer(){

}
Buffer::~Buffer(){}

/*****************/
/* VERTEX BUFFER */
/*****************/

VertexBuffer::VertexBuffer(const void* data, unsigned int size){
	// Generates the given amount of unused buffers and passes the IDs to the given int array
	glGenBuffers(1, &m_renderer_id);
	// Bind vertex buffer object
	// If buffer name doesn't exist, one is created automatically
	// Binding a buffer to a target breaks previous binding of a buffer to the given target, i.e. only one buffer per target possible
	// Setting buffer to 0 unbinds any buffer set to given target
	glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id);
	// Add triangle vertices into target buffer
	// Create NEW data store in the buffer object currently bound to given target,
	// meaning any previously stored data in the buffer is DELETED!
	// 
	// static draw: draw data, which will most likely not be mutated at any point in the program
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}
VertexBuffer::~VertexBuffer(){
	glDeleteBuffers(1, &m_renderer_id);
}

void VertexBuffer::bind() const{
	glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id);
}
void VertexBuffer::unbind() const{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/****************/
/* INDEX BUFFER */
/****************/

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_count(count){
	glGenBuffers(1, &m_renderer_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}
IndexBuffer::~IndexBuffer(){
	glDeleteBuffers(1, &m_renderer_id);
}

void IndexBuffer::bind() const{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id);
}
void IndexBuffer::unbind() const{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


/****************/
/* VERTEX ARRAY */
/****************/

VertexArray::VertexArray(){
	glGenVertexArrays(1, &m_renderer_id);
}

VertexArray::~VertexArray(){
	glDeleteVertexArrays(1, &m_renderer_id);
}

void VertexArray::add_buffer(VertexBuffer* vertex_buffer, const BufferLayout& layout){
	// Make sure VA is bound, before binding VB to GL_ARRAY_BUFFER!
	bind();
	vertex_buffer->bind();

	const std::vector<BufferElement> elements = layout.get_elements();
	BufferElement el;
	for(uint32_t i = 0; i < elements.size(); i++){
		el = elements.at(i);
		// Tell OpenGL, that we want to use part of the array beginning at the given index
		glEnableVertexAttribArray(i);
		// Tell OpenGL, how many bytes we want to read starting from the given index, and which type
		// Since data in buffer might have no real order, stride must be set to the size of the entire array,
		// while the correct position is determined through the offset
		glVertexAttribPointer(i, el.count, el.type, el.normalized, layout.get_stride(), (const void*)el.offset);
	}

	m_vertex_buffer = vertex_buffer;
	m_vertex_buffer->set_buffer_layout(layout);

}

void VertexArray::bind() const{
	glBindVertexArray(m_renderer_id);
}

void VertexArray::unbind() const{
	glBindVertexArray(0);
}
