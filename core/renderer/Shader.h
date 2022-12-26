#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

/*
IDEAS TO ORGANIZE SHADER:

- "#shader" directive determines, what kind of shader the following code will be
	- anytime a "#shader" directive is called, the following code will be processed for the given shader type
- use single main shader file containing all necessary functions and stuff
	- each "#shader" type will be treated as its own program and the code will be divided between each type
- use "#include" directives to include files from outside the main shader file
	- only files included in the main shader file, or in the files included in the main shader file will be used
	- "#include"s are replaced by entire file, so make sure there are no circular includes
	- in (and only in) the main file, "#include"s must be passed exactly once for each shader, for which they are needed
	- must set "#shader" directive in non-main files!
	- do NOT write "#include" directives multiple times in the same non-main file!
- for non-main files:
	- like in the main files, set the "#shader" directives to determine the type of shader
		- if none are specified, vertex shader is assumed
		- variables and functions cannot be used over different shader types
	- each file represents the shader for a material
	- each file contains variables and functions to be used
	- each filename is mapped to an ID
	- put variables between "#variables_start" and "#variables_end" directives
	- put functions between "#functions_start" and "#functions_end" directives"
	- do NOT use main() functions!
- for each vertex in the buffer:
	- IDs are used to determine the correct function to execute in the main shader file
	- each vertex gets the ID corresponding to the filename-ID mapping for the material they use 
	- anytime the material changes, the ID value inside the buffer changes
	- 0 is reserved for default shader
- vertex buffer layout:
	- 3d positions
	- uv coordinates
	- material ID
	- MVP ID

*/


struct ShaderSources{
	std::string vertex;
	std::string fragment;
};

class Shader{

public:
	Shader(const std::string& file);
	virtual ~Shader();

	virtual void bind() const;
	virtual void unbind() const;

	virtual void set_uniform_1i(const std::string& name, int i);
	virtual void set_uniform_4f(const std::string& name, float f1, float f2, float f3, float f4);
	virtual void set_uniform_mat4f(const std::string& name, const glm::mat4& proj);

	virtual int get_uniform_location(const std::string& name);

	uint32_t get_renderer_id() const { return m_renderer_id; };

private:
	uint32_t m_renderer_id;
	std::string m_file;
	std::unordered_map<std::string, int> m_cache;		// maybe limit cache size to avoid overhead

	ShaderSources parse_shader();
	uint32_t compile_shader(uint32_t type, const std::string& source);
	void create_shader();

};

#endif // !__SHADER_H__
