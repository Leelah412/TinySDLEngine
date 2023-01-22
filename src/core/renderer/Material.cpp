#include "Material.h"

namespace tse{


unsigned int Material::s_material_count = 0;

Material::Material(){
	m_material_id = ++s_material_count;
}
Material::Material(Shader* shader){
	m_material_id = ++s_material_count;
	set_shader(shader);
}
Material::~Material(){}

void Material::bind() const{
	// Current texture slot
	int slot = 0;

	auto it = m_uniforms.begin();
	for(it; it != m_uniforms.end(); it++){
		if(!it->second.value) continue;

		switch(it->second.uniform_type){

			// TEXTURE TYPES

			case GL_SAMPLER_1D: {
				break;
			}
			// Simple texture
			case GL_SAMPLER_2D: {
				// Cast to texture
				Texture* tex = (Texture*)it->second.value;
				// Activate texture
				tex->bind(slot);
				m_shader->set_uniform_1i(it->first, slot);
				slot++;
				break;
			}
			// Texture array
			case GL_SAMPLER_3D: {
				break;
			}
			case GL_SAMPLER_CUBE: {
				break;
			}
			case GL_SAMPLER_1D_SHADOW: {
				break;
			}
			case GL_SAMPLER_2D_SHADOW: {
				break;
			}
			default: break;
		}
	}
}

const Shader* Material::get_shader() const{ return m_shader; }
Shader* Material::get_shader_mutable() const{ return m_shader; }
void Material::set_shader(Shader* shader){
	m_shader = shader;
	init_shader_uniforms();
}

void Material::init_shader_uniforms(){
	if(!m_shader) return;
	m_uniforms = {};

	GLint num_uniforms;
	glGetProgramiv(m_shader->get_program_id(), GL_ACTIVE_UNIFORMS, &num_uniforms);
	GLchar uniform_name[256];
	GLsizei length;
	GLint size;
	GLenum type;
	for(int i = 0; i < num_uniforms; i++){
		Uniform uni;
		glGetActiveUniform(m_shader->get_program_id(), i, sizeof(uniform_name), &length, &size, &type, uniform_name);
		uni.uniform_type = type;
		uni.uniform_name = uniform_name;
		m_uniforms.insert(std::pair<std::string, Uniform>(uniform_name, uni));
	}
}

unsigned int Material::get_material_id() const{ return m_material_id; }


const Uniform* Material::get_uniform(const std::string& name) const {
	if(m_uniforms.find(name) == m_uniforms.end()) return nullptr;
	return &(*m_uniforms.find(name)).second;
}

void Material::set_uniform(const std::string& name, void* value, GLuint size){
	// TODO: 
	//m_shader.set
	//m_uniforms[uniform.uniform_name] = uniform;
}

void Material::set_uniform_value(const std::string& uniform_name, const void* new_value, unsigned int new_value_size){
	if(m_uniforms.find(uniform_name) == m_uniforms.end()) return;
	std::memcpy(m_uniforms[uniform_name].value, new_value, new_value_size);
	m_uniforms[uniform_name].value_size = new_value_size;
}

void Material::set_texture(const std::string& uniform_name, const Texture* texture){
	// Uniform must exist and be a sampler type
	if(m_uniforms.find(uniform_name) == m_uniforms.end()) return;

	switch(m_uniforms[uniform_name].uniform_type){
		case GL_SAMPLER_1D: {
			break;
		}
		// Simple texture
		case GL_SAMPLER_2D: {
			m_uniforms[uniform_name].value = (void*)texture;
			m_uniforms[uniform_name].value_size = sizeof(*texture);
			break;
		}
		// Texture array
		case GL_SAMPLER_3D: {
			break;
		}
		case GL_SAMPLER_CUBE: {
			break;
		}
		case GL_SAMPLER_1D_SHADOW: {
			break;
		}
		case GL_SAMPLER_2D_SHADOW: {
			break;
		}
		default: break;
	}
}

}