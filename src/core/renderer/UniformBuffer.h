#ifndef __UNIFORM_BUFFER_H__
#define __UNIFORM_BUFFER_H__

#include "Buffer.h"

class UniformBuffer : public Buffer{

public:
	UniformBuffer(GLuint size);
	virtual ~UniformBuffer();

	void bind() const override;
	void unbind() const override;

	// Push the given data with the given offset and size into the buffer
	bool push(const void* data, GLuint offset, GLuint size);
	// Resets data to zero starting at given offset with given size
	void pop(GLuint offset, GLuint size);
	// Allocate new buffer memory; deletes previous buffer
	bool alloc_buffer(GLuint size);

private:
	GLuint m_size;

};

#endif // !__UNIFORM_BUFFER_H__
