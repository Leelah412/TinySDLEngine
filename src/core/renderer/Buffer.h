#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "VertexData.h"

#include <GL/glew.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

/* Contains VertexArray, VertexBuffer, IndexBuffer classes */
/* Contains VertexAttributeLayout class and VertexAttribute struct */

// Buffer element corresponding to a single vertex attribute
typedef struct VertexAttribute{
	std::string name;		// Name of the variable name in the shader file (i think idk)
	GLuint type;			// OpenGL type (e.g. GL_UNSIGNED_INT)
	GLuint count;			// Number of entries for the vertex attribute
	size_t offset;			// Element offset in the array
	GLuint normalized;		// Value is normalized?

	// Return total number of bytes of the VBE
	GLuint get_bytes(){
		return get_size(type) * count;
	}

	// Return size of the OpenGL datatype
	static GLuint get_size(GLuint _type){
		switch(_type){
			case GL_UNSIGNED_INT:
			case GL_FLOAT: return 4;
			default:{
				std::cerr << "ERROR: Given VertexAttribute type does not exist!" << std::endl;
				return 0;
			}
		}
	}
} VertexAttribute;

// Layout representing the attribute order per vertex
class VertexAttributeLayout{

public:
	VertexAttributeLayout(){}
	virtual ~VertexAttributeLayout(){}

	const std::vector<VertexAttribute>& get_elements() const{ return m_elements; }
	GLuint get_stride() const{ return m_stride; }

	void push(GLuint count, GLuint type, unsigned char normalized = false){
		VertexAttribute el = VertexAttribute();
		el.name = "";
		el.type = type;
		el.count = count;
		el.offset = m_stride;
		el.normalized = normalized;

		m_elements.push_back(el);
		// don't forget to increment stride!
		m_stride += el.get_bytes();
	}

private:
	std::vector<VertexAttribute> m_elements;
	GLuint m_stride = 0;

	void calc_offsets(){
		m_stride = 0;
		for(GLuint i = 0; i < m_elements.size(); i++){
			m_elements.at(i).offset = m_stride;
			m_stride += m_elements.at(i).get_bytes();
		}
	}
};


// Base class for all buffer-type classes (incl. VertexArray)
class Buffer{

public:
	Buffer();
	virtual ~Buffer();

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	const GLuint& get_buffer_id() const { return m_buffer_id; }

protected:
	GLuint m_buffer_id;				// This is the buffer object!
};

// VertexBuffer class responsible for data management inside the corresponding VBO
// Does not care about what's inside the buffer, only responsible for allocation and writing of data into the buffer
class VertexBuffer : public Buffer{

public:
	VertexBuffer(GLuint size, bool is_static = false);
	virtual ~VertexBuffer();

	void bind() const override;
	void unbind() const override;

	// Push the given data with the given offset and size into the buffer
	bool push(const void* data, GLuint offset, GLuint size);
	// Resets data to zero starting at given offset with given size
	void pop(GLuint offset, GLuint size);
	// Allocate new buffer memory; deletes previous buffer
	bool alloc_buffer(GLuint size, bool is_static = false);				

	const VertexAttributeLayout& get_layout() const;
	void set_layout(const VertexAttributeLayout& layout);

	GLuint get_size() const;
	bool is_static() const;

private:
	VertexAttributeLayout m_layout;
	GLuint m_size;								// Complete size of the buffer
	bool m_static;								// True: static buffer, False: dynamic buffer
};

class IndexBuffer : public Buffer{

public:
	// Construct index buffer passing the max. number of possible indices, start data, and the datasize in bytes
	IndexBuffer(GLuint max_indices, bool is_static = false);
	virtual ~IndexBuffer();

	void bind() const override;
	void unbind() const override;

	// Like in VertexBuffer, but with index count instead of byte count (i.e. multiply by sizeof(GLuint))
	bool push(const void* data, GLuint index, GLuint count);
	void pop(GLuint index, GLuint count);
	// Allocate new buffer memory; deletes previous array
	bool alloc_buffer(GLuint count, bool is_static = false);

	GLuint get_max_indices() const;
	bool is_static() const;

private:
	GLuint m_max_indices;							// Max. number of indices, that can be stored
	bool m_static;
};


#endif // !__BUFFER_H__
