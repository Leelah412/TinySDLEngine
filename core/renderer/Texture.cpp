#include "Texture.h"

Texture::Texture(const std::string& file)
	: m_filepath{file}{

	stbi_set_flip_vertically_on_load(1);
	m_buffer = stbi_load(file.c_str(), &m_width, &m_height, &m_bpp, 4);

	glGenTextures(1, &m_texture_id);
	glBindTexture(GL_TEXTURE_2D, m_texture_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// don't extend area
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// no tiling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// give data to opengl
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer);
	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);

}

Texture::Texture(const Texture& copy){

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
