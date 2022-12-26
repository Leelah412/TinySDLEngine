#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <stb_image.h>
#include <GL/glew.h>
#include <string>

// Bind textures to draw them on screen, when needed

class Texture{

public:
	Texture(const std::string& file);
	virtual ~Texture();

	// Bind slot 0 for texture maps, slot 1 for normal maps, ...
	virtual void bind(uint32_t slot = 0) const;
	virtual void unbind() const;

	uint32_t get_texture_id(){ return m_texture_id; }
	const std::string& get_filepath(){ return m_filepath; }
	const unsigned char* get_buffer(){ return m_buffer; }
	int get_width(){ return m_width; }
	int get_height(){ return m_height; }
	int get_bpp(){ return m_bpp; }

private:
	// WARNING: if loaded into resource manager, any changes to any of these values will also affect every mesh using this texture!
	uint32_t m_texture_id = 0;
	std::string m_filepath;
	unsigned char* m_buffer = nullptr;
	int m_width = 0;
	int m_height = 0;
	int m_bpp = 0;
};

#endif // !__TEXTURE_H__
