#shader vertex
#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec4 normals;

out vec2 v_tex_coord;
out vec4 v_normals;

uniform mat4 U_VIEW;

void main(){
	gl_Position = U_VIEW * position;
	v_tex_coord = tex_coord;
	v_normals = normals;
};


#shader fragment
#version 420 core

layout(location = 0) out vec4 color;

in vec2 v_tex_coord;
in vec4 v_normals;

uniform vec4 u_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D u_texture;

layout(std140, binding = 0) uniform Light{
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	float constant;
    float linear;
    float quadratic;
};

void main(){
	//vec3 lightDir = normalize(-light.direction);

	vec4 tex_color = texture(u_texture, v_tex_coord);
	color = tex_color * u_color;
};