#include "Texture.h"

#include <iostream>

namespace tse{

Texture::Texture(int width, int height, TSETextureType type)
	: m_width{width}, m_height{height}, m_type{type}{
	gl_load();
}

Texture::Texture(const std::string& path, TSETextureType type) : m_type{type}{
	m_filepath = path;
	stbi_set_flip_vertically_on_load(1);
	if((m_buffer = stbi_load(m_filepath.c_str(), &m_width, &m_height, &m_bpp, 4)) == nullptr){
		std::cerr << "ERROR: Couldn't load texture!" << std::endl;
		return;
	}

	gl_load();

}

Texture::Texture(const Texture& texture){
	copy(texture);
	gl_load();
}

Texture& Texture::operator=(const Texture& texture){
	if(this == &texture) return *this;

	copy(texture);
	gl_load();

	return *this;
}

Texture::~Texture(){
	if(m_buffer) stbi_image_free(m_buffer);
	glDeleteTextures(1, &m_texture_id);
}

void Texture::bind(uint32_t slot) const{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);
}

void Texture::unbind() const{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::copy(const Texture& texture){
	// texture ID will be created by OpenGL, so don't copy that
	m_filepath = texture.m_filepath;
	m_width = texture.m_width;
	m_height = texture.m_height;
	m_bpp = texture.m_bpp;

	// copy buffer, if exists
	if(!texture.m_buffer) return;
	size_t size = (size_t)m_width * (size_t)m_height * (size_t)m_bpp;
	m_buffer = new unsigned char(size);
	if(!m_buffer){
		std::cerr << "ERROR: Couldn't assign memory to texture buffer. Buffer is null." << std::endl;
		return;
	}
	std::memcpy(m_buffer, texture.m_buffer, size);
}

void Texture::gl_load(){
	glGenTextures(1, &m_texture_id);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// don't extend area
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// no tiling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	switch(m_type){
		case TSETextureType::COLOR:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
			break;
		// TODO: look deeper into depth and stencil textures
		//case TSETextureType::DEPTH:
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, m_buffer);
		//	break;
		//case TSETextureType::STENCIL:
		//	glTexImage2D(GL_TEXTURE_2D, 0, GL_STENCIL_INDEX, m_width, m_height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, m_buffer);
		//	break;
		default:
			std::cerr << "ERROR: Couldn't create Texture: Invalid texture type!" << std::endl;
			break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

}

