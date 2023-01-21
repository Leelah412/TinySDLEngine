#include "SceneLoader.h"

#include <queue>
#include <unordered_set>
#include <unordered_map>

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

	// Load nodes using a queue to load tree bottom-up
	// This ensures, that when a parent loads a child, 
	JSON nodes = body["nodes"];
	if(nodes.empty()){
		std::cout << "No nodes, nothing to load." << std::endl;
		return;
	}

	// Key: Node name
	// Value: Node parameters
	JSON node_val;
	std::queue<std::string> q;
	std::string front;
	q.push(nodes.begin().key());
	// Nodes, which have been created already
	std::unordered_map<std::string, Node*> done;
	// Nodes, which have parents assigned to them; used to check, if a child has more than one parent (which is illegal and will cause an error!)
	std::unordered_set<std::string> has_parent_assigned;

	while(!q.empty()){
		front = q.front();
		q.pop();
		// has children -> process them first
		if(nodes[front].find("children") != nodes[front].end() && (nodes[front]["children"].size() > 0)){
			// must be an array
			if(!nodes[front]["children"].is_array()){
				std::cout << "WARNING: Incorrect implementation: Child list must be an array!" << std::endl;
				return;
			}
			for(auto& ch : nodes[front]["children"])
				q.push(ch);
			continue;
		}
	}

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
		Node* new_node = s_node_map()[node_val["class"]]();
		new_node->load(node_val);
	}
}

void SceneLoader::save_scene(JSON nodes){

}

}

