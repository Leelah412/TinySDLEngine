#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <GL/glew.h>
#include <string>
#include <vector>
#include <iostream>

/* Contains VertexArray, VertexBuffer, IndexBuffer classes */
/* Contains BufferLayout class and BufferElement struct */


struct BufferElement{
	std::string name;		// Name of the variable name in the shader file (i think idk)
	uint32_t type;			// OpenGL type (e.g. GL_UNSIGNED_INT)
	uint32_t count;			// Number of entries for the vertex attribute
	size_t offset;			// Element offset in the array
	uint32_t normalized;	// Value is normalized (between -1.0 and 1.0)

	uint32_t get_full_size(){
		return get_size(type) * count;
	}

	static uint32_t get_size(uint32_t _type){
		switch(_type){
			case GL_UNSIGNED_INT:
			case GL_FLOAT: return 4;
			default:{
				std::cerr << "ERROR: Given BufferElement type does not exist!" << std::endl;
				return 0;
			}
		}
	}
};

// Basically, a vertex buffer is just a meaningless row of bytes completely useless in its form to OpenGL,
// while a buffer layout gives information about how the data is actually structured and which parts are connected,
// where each buffer element represents a single vertex ATTRIBUTE (NOT necessarily the vertex itself!).
// A vertex attribute is e.g. the position of a vertex in the 2D plane consisting of two floats á 4 bytes,
// and the buffer element "encapsulates" parts of the vertex buffer with information about the start (offset)
// and end (offset + count * sizeof(type)) of the attribute.
class BufferLayout{

public:
	BufferLayout(){}
	virtual ~BufferLayout(){}

	const std::vector<BufferElement> get_elements() const{ return m_elements; }
	uint32_t get_stride() const{ return m_stride; }

	void push(uint32_t count, uint32_t type, unsigned char normalized = false){
		BufferElement el = BufferElement();
		el.name = "";
		el.type = type;
		el.count = count;
		el.offset = m_stride;
		el.normalized = normalized;

		m_elements.push_back(el);
		// don't forget to increment stride!
		m_stride += el.get_full_size();
	}

private:
	std::vector<BufferElement> m_elements;
	uint32_t m_stride = 0;

	void calc_offsets(){
		m_stride = 0;
		for(uint32_t i = 0; i < m_elements.size(); i++){
			// Note: offset is an unsigned long long, while m_stride is an unsigned int,
			// meaning for buffers greater than 4.2 GB, a wrap around would happen for m_stride and this is going to fail
			m_elements.at(i).offset = m_stride;
			m_stride += m_elements.at(i).get_full_size();
		}
	}
};


class Buffer{

public:
	Buffer();
	virtual ~Buffer();

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	const unsigned int& get_renderer_id() const { return m_renderer_id; }

protected:
	unsigned int m_renderer_id;				// This is the buffer object!
};


class VertexBuffer : public Buffer{

public:
	VertexBuffer(){ m_layout = BufferLayout(); }
	VertexBuffer(const void* data, unsigned int size);
	virtual ~VertexBuffer();

	void bind() const override;
	void unbind() const override;

	const BufferLayout& get_buffer_layout() const{ return m_layout; }
	void set_buffer_layout(const BufferLayout& layout){ m_layout = layout; }

private:
	BufferLayout m_layout;
};


class IndexBuffer : public Buffer{

public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	virtual ~IndexBuffer();

	void bind() const override;
	void unbind() const override;

	unsigned int get_count() const { return m_count; }

private:
	unsigned int m_count;
};

class VertexArray : public Buffer{

public:
	VertexArray();
	virtual ~VertexArray();

	void add_buffer(VertexBuffer* vertex_buffer, const BufferLayout& layout);

	void bind() const override;
	void unbind() const override;

	const VertexBuffer* get_vertex_buffer() const { return m_vertex_buffer; }

private:
	std::vector<VertexBuffer*> m_vertex_buffer;

};

#endif // !__BUFFER_H__
