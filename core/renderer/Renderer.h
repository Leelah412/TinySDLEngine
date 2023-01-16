#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "RenderManager.h"

#include <GL/glew.h>

#define USE_RENDERER extern tse::Renderer* current_renderer;
#define IRenderer current_renderer

USE_RENDER_MANAGER

namespace tse{

class Renderer{

public:
	Renderer();
	virtual ~Renderer();

	virtual void setup();
	virtual void clear();
	virtual void draw();
};


}



#endif // !__RENDERER_H__
