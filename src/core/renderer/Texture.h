#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Resource.h"

#include <stb_image.h>
#include <GL/glew.h>
#include <string>

namespace tse{

enum class TSETextureType {COLOR, DEPTH, STENCIL};

// Texture class holding information about textures (possibly) drawn onto Framebuffers
class Texture : public Resource{

public:
	// Create an empty Texture with the given width and height
	Texture(int width, int height, TSETextureType type = TSETextureType::COLOR);
	// Create a Texture from the file in the given path
	Texture(const std::string& path, TSETextureType type = TSETextureType::COLOR);
	Texture(const Texture& texture);		// Copy constructor
	Texture& operator=(const Texture& texture);
	virtual ~Texture();

	// Bind slot 0 for texture maps, slot 1 for normal maps, ...
	virtual void bind(uint32_t slot = 0) const;
	virtual void unbind() const;

	uint32_t get_texture_id(){ return m_texture_id; }
	const unsigned char* get_buffer(){ return m_buffer; }
	int get_width(){ return m_width; }
	int get_height(){ return m_height; }
	int get_bpp(){ return m_bpp; }

private:
	// WARNING: if loaded into resource manager, any changes to any of these values will also affect every mesh using this texture!
	uint32_t m_texture_id = 0;
	unsigned char* m_buffer = nullptr;
	TSETextureType m_type;
	int m_width = 0;
	int m_height = 0;
	int m_bpp = 0;

	void copy(const Texture& texture);			// For copy constructors
	void gl_load();								// Load texture into OpenGl

};

}



#endif // !__TEXTURE_H__
