#ifndef __SHADER_H__
#define __SHADER_H__

#include "Resource.h"
#include <glm/glm.hpp>
#include <GL/glew.h>

#include <string>
#include <unordered_map>

struct ShaderSources{
	std::string vertex;
	std::string fragment;
};

// TODO: don't use inheritance, but instead use names, and set name of default shader to "default"
// this will allow us to easily use the resourcemanager for loading shaders
class Shader final : public Resource{

public:
	// TODO: implement shader loading from string
	// Load the shader from the given file, or from string, if the string is the shader.
	Shader(const std::string& path, bool path_is_shader = false);
	virtual ~Shader();

	// Load up the uniform values of a shader, if needed
	//void load_uniforms();

	void bind() const;
	void unbind() const;

	void set_uniform_1i(const std::string& name, int i);
	void set_uniform_4f(const std::string& name, float f1, float f2, float f3, float f4);
	void set_uniform_mat4f(const std::string& name, const glm::mat4& proj);

	int get_uniform_location(const std::string& name);

	GLuint get_program_id() const { return m_program_id; };

private:
	GLuint m_program_id;
	std::unordered_map<std::string, int> m_cache;		// Uniform location cache

	ShaderSources parse_shader();
	GLuint compile_shader(GLenum type, const std::string& source);
	void create_shader();


};

#endif // !__SHADER_H__
