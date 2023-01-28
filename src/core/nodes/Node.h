#ifndef __NODE_H__
#define __NODE_H__

#include <Object.h>
#include <renderer/RenderManager.h>
#include "types.h"
#include <util/json/single_include/nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

#include <vector>
#include <set>
#include <queue>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>

#define NodeName(x) #x

// Event manager for node tree event.
// Anytime a parent/child is added/switched/deleted, anytime the root object is meant to be switched,
// a signal is send out to anyone wanting to react to such events.
// 
// Uses "Object"s observer system to handle signals.
// All instances wanting to listen to node tree events must derive from "Object".

#define NTEventManager tse::NodeTreeEventManager::get_default_ntem()

#define INodeTree tse::NodeTree::get_default_node_tree()
#define SetNodeTree(NT) tse::NodeTree::set_default_node_tree(NT)

namespace tse{

class Node;

typedef struct NODE_PARENT_CHILD{
	Node* parent;
	Node* child;
};
typedef union NODE_TREE_EVENT_EX{
	NODE_PARENT_CHILD parent_child;
	Node* node;
};
class NODE_TREE_EVENT : public EVENT{
public:
	NODE_TREE_EVENT(SIGNAL _signal, Object* _subject, NODE_TREE_EVENT_EX _ex): EVENT(_signal, _subject), ex{_ex}{}
	virtual ~NODE_TREE_EVENT(){};

	NODE_TREE_EVENT_EX ex;
};

class NodeTreeEventManager : public Object{

public:
	NodeTreeEventManager();
	~NodeTreeEventManager();

	// NOTIFICATIONS

	// Notify, that parent adopted a child
	// Note: Does not check, whether nodes are indeed related or whether they are part of the tree!
	void child_added(Node* parent, Node* child);
	void child_removed(Node* parent, Node* child);
	void node_to_delete(Node* node);						// notify, that node is about to be deleted

	// COMMANDS
	void switch_root(Node* new_root);						// command to switch the root to the given node

	// LISTENERS
	void listen_child_added_event(Object* listener);
	void listen_child_removed_event(Object* listener);
	void listen_node_to_delete_event(Object* listener);
	void listen_switch_root_event(Object* listener);

	void drop_child_added_event(Object* listener);
	void drop_child_removed_event(Object* listener);
	void drop_node_to_delete_event(Object* listener);
	void drop_switch_root_event(Object* listener);

	static NodeTreeEventManager* get_default_ntem();

private:
	static NodeTreeEventManager* s_default_ntem;
};

class NodeTree;

// Base class for all Node objects
class Node : public Object{
public:

	Node();
	virtual ~Node();

	// TODO: create on_create, on_tree_enter, on_destroy... functions

	virtual void input(const SDL_Event& event);			// input passed through the node tree, until a child "eats" it; return remaining event queue
	virtual void draw();								// directly draws on the screen
	virtual void update(const time_t& delta);			// updates physics n stuff

	virtual const glm::vec2& get_position_2d() const;
	virtual const glm::vec3& get_position() const;
	virtual float get_position_x() const;
	virtual float get_position_y() const;
	virtual float get_position_z() const;
	virtual void set_position_2d(const glm::vec2& position);
	virtual void set_position_2d(float x, float y);
	virtual void set_position(const glm::vec3& position);
	virtual void set_position(float x, float y, float z);
	virtual void set_position_x(float x);
	virtual void set_position_y(float y);
	virtual void set_position_z(float z);
	virtual const glm::vec2& get_global_position_2d() const;
	virtual const glm::vec3& get_global_position() const;
	virtual float get_global_position_x() const;
	virtual float get_global_position_y() const;
	virtual float get_global_position_z() const;
	virtual void update_global_position();

	virtual float get_scale() const;
	virtual void set_scale(float scale);
	virtual float get_global_scale() const;
	virtual void update_global_scale();

	// X: Pitch, Y: Yaw, Z: Roll
	virtual glm::vec3 get_rotation() const;
	virtual void set_rotation(glm::vec3 rotation);
	virtual void set_rotation(float pitch, float yaw, float roll);
	virtual void set_pitch(float pitch);
	virtual void set_yaw(float yaw);
	virtual void set_roll(float roll);
	virtual glm::vec3 get_global_rotation() const;
	virtual float get_global_pitch();
	virtual float get_global_yaw();
	virtual float get_global_roll();
	virtual void update_global_rotation();

	// Returns the global transformation matrix created by the global position, TODO scale and rotation values
	virtual glm::mat4 get_transform();
	// Returns the local transformation matrix
	virtual glm::mat4 get_local_transform();

	uint64_t get_ID() const;
	std::string get_unique_name() const;
	bool set_unique_name(std::string name);						// assign a new unique name for the node | returns false, if name already exists

	Node* get_parent() const;
	virtual void set_parent(Node* parent);
	// Switch parent and possibly keep the global dimensions of the node
	// Use this, if updating transformation of child in any way is desired
	virtual void switch_parent(Node* parent, bool keep_gl_pos = false, bool keep_gl_rot = false, bool keep_gl_scale = false);

	const std::vector<Node*>& get_children() const;
	// Add the given object as the child of the current object and return the new child
	Node* add_child(Node* child);
	// Child object stops being child of current, but still exists
	void disown_child(Node* child);
	// Delete the child and its entire existence
	void delete_child(Node* child);
	void delete_child_by_id(uint64_t id);
	void remove_all_children();						// deletes all children and their entire existence
	//std::vector<Node*> get_all_children_of_class(const std::string& class_name, bool recursive = false);		// return all children of the given class type

	virtual JSON save();										// Save Node to JSON
	virtual void load(const JSON& data);						// Load Node from JSON data

	bool is_exempt_from_saving();
	void set_exempt_from_saving(bool exempt);

	virtual std::string get_class_name();

private:
	static uint64_t m_node_count;
	static std::set<std::string> m_used_unique_names;			// set of unique names already in use to avoid duplicats

	uint64_t m_ID;
	std::string m_unique_name;									// unique name for direct access
	// FIXME: when loading scene, this will orphan the children of this node, and the nodes parent will skip these nodes,
	// which is not what we want, so before ever setting this true, we have to find a way to handle that!
	bool m_exempt_from_saving = false;							// If true, don't save Node 

	Node* m_parent;
	std::vector<Node*> m_children;

	// IMPORTANT: Do NOT allow direct access to dimensional properties, even for derived classes,
	// since with each relative change, the global dimensions also need to be altered,
	// and the calculations for them are done inside the setters!

	glm::vec3 m_position = glm::vec3();							// Position of the node relative to parent
	float m_scale = 1;											// TODO: implement 3D scale Scale of the node relative to parent
	glm::vec3 m_rotation = glm::vec3();							// X: Pitch, Y: Yaw, Z: Roll

	// global versions relative to immovable root element
	glm::vec3 m_global_position = glm::vec3();
	float m_global_scale = 1;									// TODO: implement 3D scale
	glm::vec3 m_global_rotation = glm::vec3();					// X: Pitch, Y: Yaw, Z: Roll
};


// NodeTree is responsible for the root object, scene switches and render setup
class NodeTree : public Object{

public:
	NodeTree(Node* root_node = nullptr);
	virtual ~NodeTree();

	// Pre-Scene setup TODO: deprecated
	void render_setup();

	// Deletes the entire node tree
	// Called, when setting up a completely new scene!
	void remove_tree();
	Node* get_root_node();
	void switch_root_node(Node* new_root, bool delete_irrelevant_nodes = false);

	static NodeTree* get_default_node_tree();
	static void set_default_node_tree(NodeTree* node_tree = nullptr);

private:
	Node* m_root_node;
	static NodeTree* s_default_node_tree;
};

}

#endif // !__NODE_H__