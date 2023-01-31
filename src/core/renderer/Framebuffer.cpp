#include "Framebuffer.h"

namespace tse{

Framebuffer::Framebuffer(unsigned int width, unsigned int height, char flags)
	: m_width{width}, m_height{height}, m_target_flags{flags}{
	glGenFramebuffers(1, &m_buffer_id);
	update_targets();
}

Framebuffer::~Framebuffer(){
	remove_targets();
	glDeleteFramebuffers(1, &m_buffer_id);
}

void Framebuffer::bind() const{
	glBindFramebuffer(GL_FRAMEBUFFER, m_buffer_id);
}

void Framebuffer::unbind() const{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Framebuffer::get_buffer_id() const{
	return m_buffer_id;
}

Texture* Framebuffer::get_color_target() const{
	return m_color_target;
}

void Framebuffer::create_color_target(){
	// delete previous target
	remove_color_target();
	m_color_target = new Texture(m_width, m_height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_target->get_texture_id(), 0);
}

void Framebuffer::remove_color_target(){
	delete m_color_target;
	m_color_target = nullptr;
}

Texture* Framebuffer::get_depth_target() const{
	return m_depth_target;
}

void Framebuffer::create_depth_target(){
	remove_depth_target();
	m_depth_target = new Texture(m_width, m_height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_target->get_texture_id(), 0);	
}

void Framebuffer::remove_depth_target(){
	delete m_depth_target;
	m_depth_target = nullptr;
}

Texture* Framebuffer::get_stencil_target() const{
	return m_stencil_target;
}

void Framebuffer::create_stencil_target(){
	remove_stencil_target();
	m_stencil_target = new Texture(m_width, m_height);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_stencil_target->get_texture_id(), 0);
}

void Framebuffer::remove_stencil_target(){
	delete m_stencil_target;
	m_stencil_target = nullptr;
}

void Framebuffer::update_targets(){
	bind();

	if(m_target_flags & TSE_FBO_COLOR_TARGET)
		create_color_target();
	else
		remove_color_target();

	if(m_target_flags & TSE_FBO_DEPTH_TARGET)
		create_depth_target();
	else
		remove_depth_target();

	if(m_target_flags & TSE_FBO_STENCIL_TARGET)
		create_stencil_target();
	else
		remove_stencil_target();

	unbind();
}

void Framebuffer::remove_targets(){
	remove_color_target();
	remove_depth_target();
	remove_stencil_target();
}

char Framebuffer::get_target_flags(){
	return m_target_flags;
}

void Framebuffer::set_target_flags(char flags){
	if(m_target_flags == flags) return;
	update_targets();
}

unsigned int Framebuffer::get_width() const{
	return m_width;
}

void Framebuffer::set_width(unsigned int width){
	if(m_width == width) return;
	m_width = width;
	// resize framebuffer
	update_targets();
}

unsigned int Framebuffer::get_height() const{
	return m_height;
}

void Framebuffer::set_height(unsigned int height){
	if(m_height == height) return;
	m_height = height;
	// resize framebuffer
	update_targets();
}

}
