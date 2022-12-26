#include "Renderer.h"

Renderer::Renderer(){}

Renderer::~Renderer(){}

void Renderer::render(const VertexArray& vertex_array, const IndexBuffer& index_buffer, const Shader& shader){

	// Clear canvas
	glClear(GL_COLOR_BUFFER_BIT);

	vertex_array.bind();
	index_buffer.bind();

	// Create triangles from the vertex INDICES (IBO)
	// No need to pass pointer to IBO, since we've bound it to GL_ELEMENTS_ARRAY_BUFFER
	glDrawElements(GL_TRIANGLES, index_buffer.get_count(), GL_UNSIGNED_INT, nullptr);

}
