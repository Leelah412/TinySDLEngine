#include "SceneLoader.h"

namespace tse{

//NodeMap SceneLoader::s_node_map = {};

NodeMap& SceneLoader::s_node_map(){
	static NodeMap* ans = new NodeMap();
	return *ans;
}

void SceneLoader::load_scene(const std::string& path){
	std::ifstream file(path.c_str());
	if(!file.is_open()){
		std::cout << "WARNING: Couldn't load scene from given path!" << std::endl;
		return;
	}

	JSON data = JSON::parse(file);

	file.close();

	if(!data.contains("type") || (data["type"] != "scene")){
		std::cout << "WARNING: Trying to load non-scene object as scene!" << std::endl;
		return;
	}
	if(!data.contains("body")){
		std::cout << "WARNING: Scene has no body!" << std::endl;
		return;
	}
	
	JSON body = data["body"];
	if(!body.contains("nodes")){
		std::cout << "WARNING: Scene has no nodes!" << std::endl;
		return;
	}
	if(!body["nodes"].is_object()){
		std::cout << "WARNING: Incorrect implementation!" << std::endl;
		return;
	}

	// Load nodes
	// 
	// Assumption, that nodes are saved bottom up, meaning we can immediately assign child nodes to parents, when parents are created
	// If a nodes child node doesn't exist, when node is created, assume an error in the way the data is saved, or an unwanted circular dependency
	JSON nodes = body["nodes"];
	// Key: Node name
	// Value: Node parameters
	JSON node_val;
	for(auto& nd : nodes.items()){
		node_val = nd.value();
		// Object must contain fnformation about the name of the class
		if(!node_val.contains("class")){
			std::cout << "WARNING: Incorrect implementation!" << std::endl;
			return;
		}
		if( s_node_map().find(node_val["class"]) == s_node_map().end() ){
			std::cout << "WARNING: Incorrect implementation!" << std::endl;
			return;
		}
		// Create and load the node
		JSONObject* new_node = s_node_map()[node_val["class"]]();
		new_node->load(node_val);
	}



	// Create instance of given Node type with the data we've found for the node
	//s_node_map["Node"]({});
}

void SceneLoader::save_scene(JSON nodes){

}

}

