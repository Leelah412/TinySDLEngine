#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "Texture.h"
//#include "TextureArray.h"
#include "Shader.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>

namespace tse{

typedef struct Uniform{
	// TODO: maybe do this later
#if 0
	typedef union UniformValue{
		Texture* texture2D;
		Texture** texture3D;
		bool boolean;
		float fl;
		int integer;
		unsigned int uinteger;
		glm::vec2 vec2;
		glm::vec3 vec3;
		glm::vec4 vec4;
		glm::ivec2 ivec2;
		glm::ivec3 ivec3;
		glm::ivec4 ivec4;
		glm::mat2 mat2;
		glm::mat3 mat3;
		glm::mat4 mat4;
	};
#endif

	GLenum uniform_type;
	std::string uniform_name;
	//UniformValue value;
	void* value = nullptr;
	GLuint value_size = 0;

	// only used for array types
	//bool is_array = false;
	//unsigned int array_size = 0;

	//bool is_buffer = false;
	//unsigned int buffer_size = 0;


	bool operator<(const Uniform& comp) const{
		if(uniform_type != comp.uniform_type) return this->uniform_type < comp.uniform_type;
		if(uniform_name != comp.uniform_name) return this->uniform_name < comp.uniform_name;
		return value < comp.value;

		// TODO: add more stuff
#if 0
		switch(uniform_type){
			case GL_FLOAT_VEC2:
			case GL_FLOAT_VEC3:
			case GL_FLOAT_VEC4:
			case GL_INT_VEC2:
			case GL_INT_VEC3:
			case GL_INT_VEC4:
			case GL_BOOL: return value.boolean < comp.value.boolean;
			case GL_BOOL_VEC2: return value.ivec2 == comp.value.ivec2;
			case GL_BOOL_VEC3:
			case GL_BOOL_VEC4:
			case GL_FLOAT_MAT2:
			case GL_FLOAT_MAT3:
			case GL_FLOAT_MAT4:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_2D_SHADOW: return value.texture2D < comp.value.texture2D;
			case GL_SAMPLER_3D: return value.texture3D < comp.value.texture3D;

			default: return false;
		}
#endif

	}

	bool operator==(const Uniform& comp) const{
		return *this != comp;
	}

	bool operator!=(const Uniform& comp) const{
		return (*this < comp) || (comp < *this);
	}

	template<GLenum E, typename T>
	struct UniformGetter{
		void* value(const Uniform& uni){
			switch(uni.uniform_type){

			}
		}
	};
};



enum class TEXTURE_TYPE {ALBEDO, NORMAL, METALLIC, ROUGHNESS};

// TODO: add ability to create material from path
// TODO: like for shaders, use a list of material uniform variables to create the materials, instead of using inheritance
// read material types and data from files, where shadertype/filepath/string shall also be specified
// 
// Each material can have a shader bound to it, while a shader can belong to multiple materials
class Material : public Resource{
	
public:
	Material();
	Material(Shader* shader);
	virtual ~Material();

	// Set shader uniforms
	void bind() const;

	virtual const Shader* get_shader() const;
	virtual Shader* get_shader_mutable() const;
	virtual void set_shader(Shader* shader);

	// Note: shader should have been linked beforehand!
	virtual void init_shader_uniforms();

	unsigned int get_material_id() const;

	//void load_buffer();

	// Return the uniform with the given name
	const Uniform* get_uniform(const std::string& name) const;
	// Assign a new value to the given uniform
	virtual void set_uniform(const std::string& name, void* value, GLuint size);
	// Assign a different value to the uniform
	virtual void set_uniform_value(const std::string& uniform_name, const void* new_value, unsigned int new_value_size);
	// Set sampler* uniform to the given texture
	void set_texture(const std::string& uniform_name, const Texture* texture);

protected:
	Shader* m_shader = nullptr;									// Shader bound to material
	std::unordered_map<std::string, Uniform> m_uniforms = {};	// List of uniforms the shader of a material uses; key is uniform name

private:
	static unsigned int s_material_count;
	unsigned int m_material_id;

};

// TODO: remove 
// Default material using the default shader for vast majority of use cases
class DefaultMaterial : public Material{
	
public:
	DefaultMaterial(Shader* shader) : Material(shader){}
	virtual ~DefaultMaterial(){}

	void use_albedo_texture(Texture* texture){
		m_albedo = texture;
	}
	void set_albedo_color(const glm::vec4& rgba, bool reset_texture = false){
		m_albedo_col = rgba;
		if(reset_texture) m_albedo = nullptr;
	}

protected:

	enum class CHANNEL{ R, G, B, A };
	enum class EMISSION_OPERATOR { ADD, SUB };

	// Uses first vertex attribute UV coordinates
	Texture* m_albedo = nullptr;					// Albedo (base texture of the material)
	glm::vec4 m_albedo_col = glm::vec4(1.0);


	// Everything after uses second UV coordinates, if at all
	// Textures with nullptr

	Texture* m_metallic_specular = nullptr;				// Metallic/Specular map
	CHANNEL m_metallic_specular_channel = CHANNEL::R;	// Texture channel to use to determine roughness
	float m_metallic_deg = 0.0;							// Constant metallic value
	float m_specular_deg = 0.0;							// Constant specular value
	bool m_use_specular = true;							// True: use texture as specular map/ use specular constant; False: use metallic version

	Texture* m_roughness = nullptr;						// Roughness map
	CHANNEL m_roughness_channel = CHANNEL::R;			// Texture channel to use to determine roughness
	float m_roughness_deg = 0.0;						// Constant roughness

	Texture* m_normal = nullptr;						// Normal map

	Texture* m_emission = nullptr;						// Emission map
	float m_emission_energy = 0.0;						// Energ
	glm::vec3 m_emission_color = glm::vec3();			// Color of the emission (if)
	EMISSION_OPERATOR m_emission_operator = EMISSION_OPERATOR::ADD;

	Texture* m_ao = nullptr;							// Ambient occlusion
	float m_ao_deg = 0.0;								// Constant AO value

};


}


#endif // !__MATERIAL_H__
