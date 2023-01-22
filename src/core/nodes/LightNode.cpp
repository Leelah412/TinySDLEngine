#include "LightNode.h"
#include <glm/gtx/string_cast.hpp>

namespace tse{

LightNode::LightNode(){

}
LightNode::LightNode(Light* light){
	set_light(light);
}
LightNode::LightNode(const Light& light){
	set_light(light);
}
LightNode::~LightNode(){

}

void LightNode::set_light(Light* light){
	delete m_light;
	m_light = light;
}

void LightNode::set_light(const Light& light){
	delete m_light;
	set_light(new Light(light));
}

void LightNode::update_global_position(){
	Node::update_global_position();
	if(!m_light) return;
	m_light->position = glm::vec4(get_global_position(), 1.0f);
}

void LightNode::update_global_rotation(){
	Node::update_global_rotation();
	if(!m_light) return;
	glm::vec3 rot = get_global_rotation();
	glm::vec3 dir;
	dir.x = cos(glm::radians(-rot.y)) * cos(glm::radians(rot.x));
	dir.y = sin(glm::radians(-rot.x));
	dir.z = sin(glm::radians(-rot.y)) * cos(glm::radians(rot.x));
	std::cout << glm::to_string(dir) << std::endl;
	m_light->direction = glm::vec4(glm::normalize(dir), 0.0f);
}

void LightNode::turn_on(){
	IRenderManager->activate_light(m_light);
}

void LightNode::turn_off(){
	IRenderManager->deactivate_light(m_light);
}

JSON LightNode::save(){
	JSON data = Node::save();
	if(is_exempt_from_saving()) return data;

	if(!m_light) return data;

	data["light"] = {
		{
			"position",
			{
				{"x", m_light->position.x},
				{"y", m_light->position.y},
				{"z", m_light->position.z},
				{"w", m_light->position.w}
			}
		},
		{
			"direction",
			{
				{"x", m_light->direction.x},
				{"y", m_light->direction.y},
				{"z", m_light->direction.z},
				{"w", m_light->direction.w}
			}
		},
		{
			"ambient",
			{
				{"x", m_light->ambient.x},
				{"y", m_light->ambient.y},
				{"z", m_light->ambient.z},
				{"w", m_light->ambient.w}
			}
		},
		{
			"diffuse",
			{
				{"x", m_light->diffuse.x},
				{"y", m_light->diffuse.y},
				{"z", m_light->diffuse.z},
				{"w", m_light->diffuse.w}
			}
		},
		{
			"specular",
			{
				{"x", m_light->specular.x},
				{"y", m_light->specular.y},
				{"z", m_light->specular.z},
				{"w", m_light->specular.w}
			}
		},
		{
			"constant",
			m_light->constant
		},
		{
			"linear",
			m_light->linear
		},
		{
			"quadratic",
			m_light->quadratic
		},

	};

	return data;
}

void LightNode::load(const JSON& data){
	Node::load(data);

	set_light(new Light());

	JSON light;
	if(!data.contains("light") || !(light = data["light"]).is_object()){
		std::cout << "WARNING: Loading 'LightNode': 'light' doesn't exist or is not an object!" << std::endl;
		return;
	}


	JSON it1;

	// POSITION
	if(light.contains("position") && (it1 = light["position"]).is_object()){
		if(it1.contains("x") && it1["x"].is_number()){
			m_light->position.x = it1["x"];
		}
		else{
			m_light->position.x = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.position.x' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("y") && it1["y"].is_number()){
			m_light->position.y = it1["y"];
		}
		else{
			m_light->position.y = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.position.y' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("z") && it1["z"].is_number()){
			m_light->position.z = it1["z"];
		}
		else{
			m_light->position.z = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.position.z' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("w") && it1["w"].is_number()){
			m_light->position.w = it1["w"];
		}
		else{
			m_light->position.w = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.position.w' doesn't exist or is not a number!" << std::endl;
		}
	}
	else{
		m_light->position = glm::vec4(0.0f);
		std::cout << "WARNING: Loading 'LightNode': 'light.position' doesn't exist or is not an object!" << std::endl;
	}

	// DIRECTION
	if(light.contains("direction") && (it1 = light["direction"]).is_object()){
		if(it1.contains("x") && it1["x"].is_number()){
			m_light->direction.x = it1["x"];
		}
		else{
			m_light->direction.x = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.direction.x' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("y") && it1["y"].is_number()){
			m_light->direction.y = it1["y"];
		}
		else{
			m_light->direction.y = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.direction.y' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("z") && it1["z"].is_number()){
			m_light->direction.z = it1["z"];
		}
		else{
			m_light->direction.z = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.direction.z' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("w") && it1["w"].is_number()){
			m_light->direction.w = it1["w"];
		}
		else{
			m_light->direction.w = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.direction.w' doesn't exist or is not a number!" << std::endl;
		}
	}
	else{
		m_light->direction = glm::vec4(0.0f);
		std::cout << "WARNING: Loading 'LightNode': 'light.direction' doesn't exist or is not an object!" << std::endl;
	}

	// AMBIENT
	if(light.contains("ambient") && (it1 = light["ambient"]).is_object()){
		if(it1.contains("x") && it1["x"].is_number()){
			m_light->ambient.x = it1["x"];
		}
		else{
			m_light->ambient.x = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.ambient.x' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("y") && it1["y"].is_number()){
			m_light->ambient.y = it1["y"];
		}
		else{
			m_light->ambient.y = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.ambient.y' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("z") && it1["z"].is_number()){
			m_light->ambient.z = it1["z"];
		}
		else{
			m_light->ambient.z = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.ambient.z' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("w") && it1["w"].is_number()){
			m_light->ambient.w = it1["w"];
		}
		else{
			m_light->ambient.w = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.ambient.w' doesn't exist or is not a number!" << std::endl;
		}
	}
	else{
		m_light->ambient = glm::vec4(0.0f);
		std::cout << "WARNING: Loading 'LightNode': 'light.ambient' doesn't exist or is not an object!" << std::endl;
	}

	// DIFFUSE
	if(light.contains("diffuse") && (it1 = light["diffuse"]).is_object()){
		if(it1.contains("x") && it1["x"].is_number()){
			m_light->diffuse.x = it1["x"];
		}
		else{
			m_light->diffuse.x = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.diffuse.x' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("y") && it1["y"].is_number()){
			m_light->diffuse.y = it1["y"];
		}
		else{
			m_light->diffuse.y = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.diffuse.y' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("z") && it1["z"].is_number()){
			m_light->diffuse.z = it1["z"];
		}
		else{
			m_light->diffuse.z = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.diffuse.z' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("w") && it1["w"].is_number()){
			m_light->diffuse.w = it1["w"];
		}
		else{
			m_light->diffuse.w = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.diffuse.w' doesn't exist or is not a number!" << std::endl;
		}
	}
	else{
		m_light->diffuse = glm::vec4(0.0f);
		std::cout << "WARNING: Loading 'LightNode': 'light.diffuse' doesn't exist or is not an object!" << std::endl;
	}

	// SPECULAR
	if(light.contains("specular") && (it1 = light["specular"]).is_object()){
		if(it1.contains("x") && it1["x"].is_number()){
			m_light->specular.x = it1["x"];
		}
		else{
			m_light->specular.x = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.specular.x' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("y") && it1["y"].is_number()){
			m_light->specular.y = it1["y"];
		}
		else{
			m_light->specular.y = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.specular.y' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("z") && it1["z"].is_number()){
			m_light->specular.z = it1["z"];
		}
		else{
			m_light->specular.z = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.specular.z' doesn't exist or is not a number!" << std::endl;
		}

		if(it1.contains("w") && it1["w"].is_number()){
			m_light->specular.w = it1["w"];
		}
		else{
			m_light->specular.w = 0.0f;
			std::cout << "WARNING: Loading 'LightNode': 'light.specular.w' doesn't exist or is not a number!" << std::endl;
		}
	}
	else{
		m_light->specular = glm::vec4(0.0f);
		std::cout << "WARNING: Loading 'LightNode': 'light.specular' doesn't exist or is not an object!" << std::endl;
	}

	// CONSTANT
	if(light.contains("constant") && (it1 = light["constant"]).is_number()){
		m_light->constant = it1;
	}
	else{
		m_light->constant = 1.0f;
		std::cout << "WARNING: Loading 'LightNode': 'light.constant' doesn't exist or is not a number!" << std::endl;
	}

	// LINEAR
	if(light.contains("linear") && (it1 = light["linear"]).is_number()){
		m_light->linear = it1;
	}
	else{
		m_light->linear = 0.0f;
		std::cout << "WARNING: Loading 'LightNode': 'light.linear' doesn't exist or is not a number!" << std::endl;
	}

	// QUADRATIC
	if(light.contains("quadratic") && (it1 = light["quadratic"]).is_number()){
		m_light->quadratic = it1;
	}
	else{
		m_light->quadratic = 0.0f;
		std::cout << "WARNING: Loading 'LightNode': 'light.quadratic' doesn't exist or is not a number!" << std::endl;
	}
}

std::string LightNode::get_class_name(){
	return NodeName(LightNode);
}

}