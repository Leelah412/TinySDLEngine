#include "SceneLoader.h"

#include <queue>
#include <deque>
#include <unordered_set>
#include <unordered_map>

namespace tse{


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
		std::cout << "WARNING: Incorrect implementation or data: 'body.nodes' must be an object!" << std::endl;
		return;
	}

	// Load nodes using a queue to load tree bottom-up
	// This ensures, that when a parent loads a child, 
	JSON nodes = body["nodes"];
	if(nodes.empty()){
		std::cout << "No nodes, nothing to load." << std::endl;
		return;
	}

	if(!body.contains("root")){
		std::cout << "WARNING: Scene has no root!" << std::endl;
		return;
	}
	// Root node of the scene
	JSON root_node = body["root"];
	if(!root_node.is_string()){
		std::cout << "WARNING: Incorrect implementation or data: 'body.root' must be a string!" << std::endl;
		return;
	}

	std::deque<std::string> q;
	std::string front;
	// Key: Node name
	// Value: Node parameters
	JSON node_val, children, class_name;
	// Nodes, which have been created already
	std::unordered_map<std::string, Node*> done;
	// List of Node names, whose children have already been pushed to the deque and don't need to be checked again
	std::unordered_set<std::string> parents_checked_once;

	// Start with root node
	q.push_front(root_node);

	// For each currently processed node, push its children to the front of a deque, if it has any.
	// Only, if all children of a node have been processed, do we create an instance for the current node.
	while(!q.empty()){
		front = q.front();
		if(!nodes.contains(front)){
			std::cout << "WARNING: Incomplete data: Node " << front << " not in list!" << std::endl;
			// TODO: delete all already-created nodes
			return;
		}
		node_val = nodes[front];

		// Check children first
		children = {};
		if(node_val.find("children") != node_val.end()){
			children = node_val["children"];
			if(!children.is_array()){
				std::cout << "WARNING: Incorrect implementation or data: 'children' must be an array!" << std::endl;
				// TODO: delete all already-created nodes
				return;
			}
		}
		if(parents_checked_once.find(front) == parents_checked_once.end()){
			parents_checked_once.insert(front);
			// Has children -> process them first!
			if(!children.empty()){
				for(auto& ch : children){
					if(!ch.is_string()){
						std::cout << "WARNING: Incorrect implementation or data: 'children' must be an array of strings!" << std::endl;
						// TODO: delete all already-created nodes
						return;
					}
					q.push_front(ch);
				}

				continue;
			}
		}
		
		// Create Node

		// Object must contain its own name and it must be equivalent to key name
		if(node_val.contains("name")){
			std::cout << "WARNING: Incorrect implementation or data: Node must contain 'name' string!" << std::endl;
			// TODO: delete all already-created nodes
			return;
		}
		if(!node_val["name"].is_string()){
			std::cout << "WARNING: Incorrect implementation or data: 'name' must be a string!" << std::endl;
			// TODO: delete all already-created nodes
			return;
		}
		if(node_val["name"] != front){
			std::cout << "WARNING: Incorrect implementation or data: 'name' string must be equivalent to key!" << std::endl;
			// TODO: delete all already-created nodes
			return;
		}

		// Object must contain information about the name of the class
		if(!node_val.contains("class")){
			std::cout << "WARNING: Incorrect implementation or data: Node must contain 'class' string!" << std::endl;
			// TODO: delete all already-created nodes
			return;
		}

		class_name = node_val["class"];
		if(!class_name.is_string()){
			std::cout << "WARNING: Incorrect implementation or data: 'class' must be a string!" << std::endl;
			// TODO: delete all already-created nodes
			return;
		}
		if(s_node_map().find(class_name) == s_node_map().end()){
			std::cout << "WARNING: Class '" << class_name << "' not registered for loading!" << std::endl;
			// TODO: delete all already-created nodes
			return;
		}

		// Create and load the Node
		Node* new_node = s_node_map()[class_name]();
		new_node->load(node_val);
		// Add children of Node
		for(auto& ch : children){
			// If child node still doesn't exist for some reason, skip and output a warning, but do not return
			if(done.find(ch) == done.end()){
				std::cout << "WARNING: Trying to adopt child, that doesn't exist! Try next child." << std::endl;
				continue;
			}
			// Call "switch_parent" instead of "add_parent", since we want the child to have its transformation updated based on new parent immediately
			new_node->switch_parent(done[ch]);
		}

		done.insert(std::pair<std::string, Node*>(front, new_node));
		q.pop_front();
	}

	// Switch root node to finalize scene
	INodeTree->switch_root_node(done[root_node], true);

	//for(auto& nd : nodes.items()){
	//	node_val = nd.value();
	//	// Object must contain fnformation about the name of the class
	//	if(!node_val.contains("class")){
	//		std::cout << "WARNING: Incorrect implementation or data: Node must contain 'class' string!" << std::endl;
	//		return;
	//	}
	//	if( s_node_map().find(node_val["class"]) == s_node_map().end() ){
	//		std::cout << "WARNING: Class '" << node_val["class"] << "' not registered for loading!" << std::endl;
	//		return;
	//	}
	//	// Create and load the node
	//	Node* new_node = s_node_map()[node_val["class"]]();
	//	new_node->load(node_val);
	//}
}

void SceneLoader::save_scene(const std::string& path){

	std::ofstream o(path.c_str());
	if(!o.is_open()){
		std::cerr << "ERROR: Couldn't open path to save Scene! Abort." << std::endl;
		return;
	}

	Node* root = INodeTree->get_root_node();

	std::queue<Node*> q;
	q.push(root);

	JSON nodes = JSON::object();
	std::map<std::string, JSON> node_map;
	Node* front;
	while(!q.empty()){
		front = q.front();
		q.pop();

		for(auto ch : front->get_children()){
			q.push(ch);
		}
		node_map.insert(std::pair<std::string, JSON>(front->get_unique_name(), front->save()));
	}

	JSON data = {
		{"type", "scene"},
		{"body",
			{
				{"root", root->get_unique_name()},
				{"nodes", node_map}
			}
		}
	};

	o << std::setw(4) << data << std::endl;
	o.close();

}


}

