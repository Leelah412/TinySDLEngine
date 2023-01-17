#ifndef __VERTEX_ARRAY_H__
#define __VERTEX_ARRAY_H__

#include "Buffer.h"
#include "Shader.h"

//#include "BufferEntity.h"

namespace tse{

// VertexArray class holding a single VertexBuffer and an IndexBuffer.
class VertexArray : public Buffer{

public:
	VertexArray();
	virtual ~VertexArray();

	void bind() const override;
	void unbind() const override;
	// Bind attributes of VBO to shader
	void bind_shader(Shader* shader, bool rebind = false);
	// Only removes shader from list of bound shaders
	void unbind_shader(Shader* shader);

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


#ifdef __BUFFER_OBJECT_H__
	// Update buffer entities
	void update();
	// Render all buffer entities connected to this array
	void render() const;

	// Push, replace or erase buffer entities
	void add_buffer_entity(BufferEntity* entity, const std::string& name, int index = -1);
	void remove_buffer_entity(unsigned int index);
	void remove_buffer_entity(const std::string& name);
	void remove_buffer_entity(BufferEntity* entity);

	const std::vector<BufferEntity*>& get_buffer_entity_list() const;
	BufferEntity* get_buffer_entity(BufferEntity* entity) const;
	BufferEntity* get_buffer_entity(unsigned int index) const;
	const std::string& get_buffer_entity_name(BufferEntity* entity) const;
	const std::string& get_buffer_entity_name(unsigned int index) const;


	BufferEntity* operator[](unsigned int index);

	std::vector<BufferEntity*>::iterator begin();
	std::vector<BufferEntity*>::iterator end();

	const std::map<std::string, unsigned int>& get_entity_names() const;
#endif

private:
	VertexBuffer* m_vbo = nullptr;
	IndexBuffer* m_ibo = nullptr;
	std::set<Shader*> m_bound_shaders;			// Shaders, to which we've been the attribute locations of the current VBO (to avoid multiple bounds)
};

}




#endif // !__VERTEX_ARRAY_H__

