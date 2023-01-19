#shader vertex
#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec3 normal;

out vec3 v_position;
out vec2 v_tex_coord;
out vec3 v_normal;

uniform mat4 U_VIEW;

void main(){
	gl_Position = U_VIEW * position;
	v_position = position.xyz;
    v_tex_coord = tex_coord;
	v_normal = normal;
};


#shader fragment
#version 420 core

layout(location = 0) out vec4 color;

in vec3 v_position;
in vec2 v_tex_coord;
in vec3 v_normal;

//uniform vec4 u_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D u_texture;
uniform mat4 U_VIEW;
uniform vec4 u_view_pos;

struct Light{
    vec4 position;
    vec4 direction;
  
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
	
	float constant;
    float linear;
    float quadratic;
    float dummy;
};

layout(std140, binding = 0) uniform LightBlock{
    Light u_light[1];
};

void main(){
	//vec3 lightDir = normalize(-light.direction);

	vec4 tex_color = texture(u_texture, v_tex_coord);
	vec3 amb = u_light[0].ambient.rgb * tex_color.rgb;
        
    // diffuse 
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(u_light[0].position.xyz - v_position);
    
    //if(u_light[0].direction.w == 0.0){
    //    lightDir = normalize(-u_light[0].direction.xyz);
    //}
    //else{
    //    lightDir = normalize(u_light[0].direction.xyz - v_position);
    //}
    
    float diff = max(dot(norm, lightDir), 0.0);
    // TODO: when implementing materials, use this
    //vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb; 
    vec3 dfs = u_light[0].diffuse.rgb * diff;
    //
    // specular
    //vec3 viewPos = vec3(-2.0f, 2.0f, 0.0f);
    vec3 viewDir = normalize(u_view_pos.xyz - v_position);
    vec3 reflectDir = reflect(-lightDir, norm);  
    //float spec = pow(max(dot(vec3(U_VIEW.m03, U_VIEW.m13, U_VIEW.m23), reflectDir), 0.0), material.shininess);
    //vec3 specular = u_light[0].specular * spec * texture(material.specular, TexCoords).rgb;  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
    vec3 specular = u_light[0].specular.rgb * spec; 
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-u_light[0].direction.xyz)); 
    //float epsilon = (light.cutOff - light.outerCutOff);
    //float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    float epsilon = (0.91 - 0.82);
    float intensity = clamp((theta - 0.82) / epsilon, 0.0, 1.0);
    dfs  *= intensity;
    specular *= intensity;
    
    // attenuation
    float dist = length(lightDir - v_position);
    float attenuation = 1.0 / (u_light[0].constant + u_light[0].linear * dist + u_light[0].quadratic * (dist * dist));
    amb *= attenuation; 
    dfs *= attenuation;
    specular *= attenuation;   
    //    
    vec3 result = amb + dfs + specular;
    //FragColor = vec4(result, 1.0);


   	color = vec4(result, 1.0);
	//color = tex_color * u_color;
};