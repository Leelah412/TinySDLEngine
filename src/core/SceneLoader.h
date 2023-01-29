#ifndef __SCENE_LOADER_H__
#define __SCENE_LOADER_H__

#include "nodes/Node.h"
#include "util/types.h"
#include <json/single_include/nlohmann/json.hpp>

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>

// Register "Node" derived class to be able to load class instances from Scene files
#define REGISTER_NODE(T)					\
	class T;								\
	namespace{								\
		tse::RegisterNode<T> reg_##T(#T);	\
	}

namespace tse{

// TODO: try with "Node" instead of "Node", and see, if circular dependency causes any problems
// if not, replace Node with Node and remove Node
using SceneNodeCreator = Node* (*)();
using NodeMap = std::unordered_map<std::string, SceneNodeCreator>;

// Creates a scene from the given JSON file
class SceneLoader{

public:
	// Load the scene from the given path
	static void load_scene(const std::string& path);
	// Save the scene based on the current node tree to the given path
	static void save_scene(const std::string& path);

protected:
	static void register_node(const std::string& class_name, SceneNodeCreator reg_func){
		if(s_node_map().find(class_name) != s_node_map().end()) return;
		s_node_map().insert(std::pair<std::string, SceneNodeCreator>(class_name, reg_func));
		std::cout << "Registered class #" << s_node_map().size() << " '" << class_name << "' in scene loader." << std::endl;
	}

private:
	static NodeMap& s_node_map();
};


// Node creation function
template <typename T>
Node* create_instance(){
	return new T();
}

// Register node class
template <typename T>
class RegisterNode : public SceneLoader{

public:
	RegisterNode(const std::string& class_name){
		register_node(class_name, &create_instance<T>);
	}

};

// Unlike its derivatives, "Node" must be registered here, since otherwise we'd have to include "SceneLoader" in "Node",
// which would cause a circular dependency
REGISTER_NODE(Node);

}


#endif // !__SCENE_LOADER_H__
