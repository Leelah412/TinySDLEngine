#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;

out vec2 v_tex_coord;

uniform mat4 U_VIEW;

void main(){
	gl_Position = U_VIEW * position;
	v_tex_coord = tex_coord;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_tex_coord;

uniform vec4 u_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D u_texture;

void main(){
	vec4 tex_color = texture(u_texture, v_tex_coord);
	color = tex_color * u_color;
};