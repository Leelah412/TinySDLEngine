#ifndef __TEXTURE_ARRAY_H__
#define __TEXTURE_ARRAY_H__

#include "Texture.h"

#include <GL/glew.h>
#include <vector>

namespace tse{


// Holds texture arrays with constant dimensions.
// Passing textures with varying sizes might cause undefined behaviour up to crashes.
template<unsigned char dim, unsigned short size, unsigned int format>
class TextureArray{

public:
	TextureArray(){
		glGenTextures(1, &m_texture_id);
	}
	virtual ~TextureArray(){
		glDeleteTextures(1, &m_texture_id);
	}

	void bind(){
		glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_id);
	}
	void unbind(){
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	}

	// Create a dataless array, which can contain the given amount of textures
	// Deletes previous array
	// Returns "true" at success
	virtual bool create_buffer(unsigned short count){
		delete m_texture_data;
		unsigned long long _size = count * get_texture_size();
		m_texture_data = new unsigned char[_size];
		std::memset(m_texture_data, 0, _size);
		m_count = count;
	}
	// Resizes buffer to fit the given number of textures
	// Returns "true" at success
	virtual bool resize_buffer(unsigned short count){

	}
	// Create texture array from a char array
	// Returns "true" at success
	virtual bool create_textures(const unsigned char* textures, unsigned short count){
		delete m_texture_data;
		unsigned long long _size = count * get_texture_size();
		m_texture_data = new unsigned char[_size];
		std::memcpy(m_texture_data, textures, _size);
		m_count = count;
		return true;
	}
	// Set or replace a texture with given textures at given index
	// Sets nothing, if trying to write out of scope
	// Returns "true" at success
	virtual bool set_texture(unsigned short index, const unsigned char* texture, unsigned short count){
		if(!count) return false;
		if((index + count) > m_count) return false;
		unsigned long long _size = count * get_pixel_size() * (unsigned long long)pow(format, dim);
		std::memcpy(m_texture_data + (index * get_texture_size()), texture, count * get_texture_size());
		return true;
	}
	// Removes textures from the array
	// Returns "true" at success
	virtual bool erase_texture(unsigned short index, unsigned short count){
		
		return true;
	}


	unsigned int get_pixel_size(){
		switch(format){
			// 4 bytes
		case GL_RGBA:
		case GL_BGRA:
			return 4;
			// 3 bytes
		case GL_RGB:
		case GL_BGR:
			return 3;
		default: return 0;
		}
	}

	// Returns the byte size of a texture with the given size, format and dimension
	unsigned int get_texture_size(){
		return get_pixel_size() * (unsigned long long)pow(format, dim);
	}

	unsigned int get_count() const{ return m_count; }

protected:
	GLuint m_texture_id;
	unsigned char* m_texture_data = nullptr;
	unsigned int m_count = 0;
};


// Creating 2D textures with a vector of varyingly sized Texture instances will NOT cause undefined behaviour,
// but empty pixels will be added/ additional pixels will be ignored, if Texture size is lower/higher than constant size
template<unsigned short size, unsigned int format>
class TextureArray2D : public TextureArray<2, size, format>{

public:
	TextureArray2D();
	virtual ~TextureArray2D();

	const unsigned char* get_textures() const;
	// Create texture array from a list of textures
	virtual void create_textures(const std::vector<Texture*>& textures);
	// Add a texture to the array
	virtual void add_texture(const unsigned char* texture);
	// Remove texture at given index
	// Since sizes are constant, index will be always at (index * size * size * )
	virtual void remove_texture(unsigned short index);

private:

};

// 2D Arrays RGBA (default)
typedef TextureArray2D<16, GL_RGBA> TextureArray2D_16;
typedef TextureArray2D<32, GL_RGBA> TextureArray2D_32;
typedef TextureArray2D<64, GL_RGBA> TextureArray2D_64;
typedef TextureArray2D<128, GL_RGBA> TextureArray2D_128;
typedef TextureArray2D<256, GL_RGBA> TextureArray2D_256;
typedef TextureArray2D<512, GL_RGBA> TextureArray2D_512;
typedef TextureArray2D<1024, GL_RGBA> TextureArray2D_1024;
typedef TextureArray2D<2048, GL_RGBA> TextureArray2D_2048;

// 2D Arrays RGB
typedef TextureArray2D<16, GL_RGB> TextureArray2D_16_RGB;
typedef TextureArray2D<32, GL_RGB> TextureArray2D_32_RGB;
typedef TextureArray2D<64, GL_RGB> TextureArray2D_64_RGB;
typedef TextureArray2D<128, GL_RGB> TextureArray2D_128_RGB;
typedef TextureArray2D<256, GL_RGB> TextureArray2D_256_RGB;
typedef TextureArray2D<512, GL_RGB> TextureArray2D_512_RGB;
typedef TextureArray2D<1024, GL_RGB> TextureArray2D_1024_RGB;
typedef TextureArray2D<2048, GL_RGB> TextureArray2D_2048_RGB;

}


#endif // !__TEXTURE_ARRAY_H__
