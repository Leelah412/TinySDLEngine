#ifndef __UNIFORM_BUFFER_H__
#define __UNIFORM_BUFFER_H__

#include "Buffer.h"

class UniformBuffer : public Buffer{

public:
	UniformBuffer(GLuint size, GLuint binding);
	virtual ~UniformBuffer();

	virtual void bind() const override;
	virtual void unbind() const override;

	virtual void rebind(GLuint binding);

	virtual bool alloc_buffer(GLuint size, GLuint binding);

private:
	GLuint m_size;
	GLuint m_binding;

};

#endif // !__UNIFORM_BUFFER_H__
