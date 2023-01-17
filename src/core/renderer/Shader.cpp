#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& path, bool path_is_shader){
	m_filepath = path;
	m_program_id = 0;
	// Create shader program for vertex and fragment shaders
	create_shader();
}

Shader::~Shader(){
	glDeleteProgram(m_program_id);
}

void Shader::bind() const{
	glUseProgram(m_program_id);
}

void Shader::unbind() const{
	glUseProgram(0);
}

void Shader::set_uniform_1i(const std::string& name, int i){
	glProgramUniform1i(m_program_id, get_uniform_location(name), i);
}

void Shader::set_uniform_4f(const std::string& name, float f1, float f2, float f3, float f4){
	glProgramUniform4f(m_program_id, get_uniform_location(name), f1, f2, f3, f4);
}

void Shader::set_uniform_mat4f(const std::string& name, const glm::mat4& proj){
	glProgramUniformMatrix4fv(m_program_id, get_uniform_location(name), 1, GL_FALSE, &proj[0][0]);
}

int Shader::get_uniform_location(const std::string& name){
	// return cached position, if exists and valid
	if( (m_cache.find(name) != m_cache.end()) && (m_cache[name] != -1) )
		return m_cache[name];

	if((m_cache[name] = glGetUniformLocation(m_program_id, name.c_str())) == -1)
		std::cout << "WARNING: Uniform " << name << " doesn't exists!" << std::endl;

	// don't worry about accessing the map multiple times, it's a hash map
	return m_cache[name];
}

ShaderSources Shader::parse_shader(){
	std::ifstream stream(m_filepath);
	if(!stream.is_open()){
		std::cerr << "ERROR: Couldn't open shader file at '" << m_filepath << "'." << std::endl;
		return {"", ""};
	}
	std::string line;
	std::stringstream ss[2];
	int mode = -1;	// -1: no mode, 0: vertex, 1: fragment

	while(getline(stream, line)){
		if(line.find("#shader") != std::string::npos){
			if(line.find("vertex") != std::string::npos){
				mode = 0;
			}
			else if(line.find("fragment") != std::string::npos){
				mode = 1;
			}
		}
		else if(mode >= 0){
			ss[mode] << line << '\n';
		}
	}

	ShaderSources s = {ss[0].str(), ss[1].str()};
	return s;
}

GLuint Shader::compile_shader(GLenum type, const std::string& source){
	// 1. Create a shader object holding the source code defining the shader
	GLuint id = glCreateShader(type);
	const char* src = source.c_str();
	// 2. Set given source string as source code of shader
	glShaderSource(id, 1, &src, nullptr);
	// 3. Compile shader
	glCompileShader(id);

	// 4. Check, if compilation was successfull
	int res;
	// iv: integer vector
	glGetShaderiv(id, GL_COMPILE_STATUS, &res);
	if(!res){
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "ERROR: Compiling shader failed: " << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	// 5. Compilation successful: return shader id
	return id;
}

void Shader::create_shader(){
	ShaderSources ss = parse_shader();

	// 1. Create program, which will execute the shaders attached to it
	m_program_id = glCreateProgram();

	// 2. Create vertex and fragment shader
	unsigned int vs = compile_shader(GL_VERTEX_SHADER, ss.vertex);
	unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, ss.fragment);
	// 3. Attach shaders to program
	glAttachShader(m_program_id, vs);
	glAttachShader(m_program_id, fs);
	// 4. Link program
	glLinkProgram(m_program_id);
	// 5. Validate program
	glValidateProgram(m_program_id);

	// 6. Free memory from shader objects, since we have them in the program now and don't need them
	glDeleteShader(vs);
	glDeleteShader(fs);

}
