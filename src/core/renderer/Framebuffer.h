#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "Texture.h"
#include <GL/glew.h>

#define TSE_FBO_COLOR_TARGET 1 << 0
#define TSE_FBO_DEPTH_TARGET 1 << 1
#define TSE_FBO_STENCIL_TARGET 1 << 2

namespace tse{

class Framebuffer{

public:
	Framebuffer(unsigned int width, unsigned int height, char flags = TSE_FBO_COLOR_TARGET);
	virtual ~Framebuffer();

	virtual void bind() const;
	virtual void unbind() const;

	GLuint get_buffer_id() const;

	Texture* get_color_target() const;
	virtual void create_color_target();
	virtual void remove_color_target();

	Texture* get_depth_target() const;
	virtual void create_depth_target();
	virtual void remove_depth_target();

	Texture* get_stencil_target() const;
	virtual void create_stencil_target();
	virtual void remove_stencil_target();

	// Creates or removes Framebuffer targets, depending on target flags
	virtual void update_targets();
	virtual void remove_targets();

	char get_target_flags();
	virtual void set_target_flags(char flags);

	unsigned int get_width() const;
	virtual void set_width(unsigned int width);
	unsigned int get_height() const;
	virtual void set_height(unsigned int height);

private:
	GLuint m_buffer_id;
	Texture* m_color_target = nullptr;
	Texture* m_depth_target = nullptr;
	Texture* m_stencil_target = nullptr;
	char m_target_flags;

	unsigned int m_width;
	unsigned int m_height;


};

}


#endif // !__FRAMEBUFFER_H__
