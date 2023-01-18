#shader vertex
#version 420 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec3 normal;

out vec4 v_position;
out vec2 v_tex_coord;
out vec3 v_normal;

uniform mat4 U_VIEW;

void main(){
	gl_Position = U_VIEW * position;
	v_position = position;
    v_tex_coord = tex_coord;
	v_normal = normal;
};


#shader fragment
#version 420 core

layout(location = 0) out vec4 color;

in vec4 v_position;
in vec2 v_tex_coord;
in vec3 v_normal;

//uniform vec4 u_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D u_texture;
uniform mat4 U_VIEW;

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
	vec3 amb = ambient * tex_color.rgb;
        
    // diffuse 
    //vec3 norm = normalize(v_normal);
    //vec3 lightDir;
    //
    //if(direction.w == 0.0){
    //    lightDir = normalize(-light.direction)
    //}
    //else{
    //    lightDir = normalize(light.position - v_position);
    //}
    //
    //float diff = max(dot(norm, lightDir), 0.0);
    //// TODO: when implementing materials, use this
    ////vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb; 
    //vec3 dfs = diffuse * diff;  
    //
    //// specular
    //vec3 viewDir = normalize(U_VIEW - v_position);
    //vec3 reflectDir = reflect(-lightDir, norm);  
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
    //
    //// spotlight (soft edges)
    //float theta = dot(lightDir, normalize(-light.direction)); 
    //float epsilon = (light.cutOff - light.outerCutOff);
    //float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    //diffuse  *= intensity;
    //specular *= intensity;
    //
    //// attenuation
    //float distance    = length(position - FragPos);
    //float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));    
    //ambient  *= attenuation; 
    //diffuse   *= attenuation;
    //specular *= attenuation;   
    //    
    //vec3 result = ambient + diffuse + specular;
    //FragColor = vec4(result, 1.0);


   	color = vec4(amb, 1.0);
	//color = tex_color * u_color;
};