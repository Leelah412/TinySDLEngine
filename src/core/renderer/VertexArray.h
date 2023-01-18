#ifndef __VERTEX_ARRAY_H__
#define __VERTEX_ARRAY_H__

#include "Buffer.h"

namespace tse{

// VertexArray class holding a single VertexBuffer and an IndexBuffer.
class VertexArray : public Buffer{

public:
	VertexArray();
	virtual ~VertexArray();

	void bind() const override;
	void unbind() const override;

	/*****************/
	/* VERTEX BUFFER */
	/*****************/
	const VertexBuffer* get_vertex_buffer() const;
	// Create an empty vertex buffer with the given size and attribute layout
	void create_vertex_buffer(GLuint size, const VertexAttributeLayout& layout, bool is_static = false);
	void set_vertex_buffer(VertexBuffer* vbo);
	bool push_vertex_buffer(const void* data, GLuint offset, GLuint size);
	// Resets data to zero starting at given offset with given size
	void pop_vertex_buffer(GLuint offset, GLuint size);
	// Allocate new buffer memory; deletes previous buffer
	bool alloc_vertex_buffer(GLuint size, bool is_static = false);
	void set_layout(const VertexAttributeLayout& layout);


	/****************/
	/* INDEX BUFFER */
	/****************/

	const IndexBuffer* get_index_buffer() const;
	// Create an empty index buffer with the given size and attribute layout
	void create_index_buffer(GLuint count, bool is_static = false);
	void set_index_buffer(IndexBuffer* ibo);
	// Like in VertexBuffer, but with index count instead of byte count (i.e. multiply by sizeof(GLuint))
	bool push_index_buffer(const void* data, GLuint index, GLuint count);
	void pop_index_buffer(GLuint index, GLuint count);
	// Allocate new buffer memory; deletes previous array
	bool alloc_index_buffer(GLuint count, bool is_static = false);

private:
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;
};

}




#endif // !__VERTEX_ARRAY_H__

