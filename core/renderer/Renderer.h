#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "Buffer.h"
#include "Shader.h"

#include <GL/glew.h>

class Renderer{

public:
	Renderer();
	virtual ~Renderer();

	static void render(const VertexArray& vertex_array, const IndexBuffer& index_buffer, const Shader& shader);

private:

};

#endif // !__RENDERER_H__
