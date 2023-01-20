#ifndef __NODE_H__
#define __NODE_H__

#include <SDLData.h>
#include <Object.h>
#include <renderer/RenderManager.h>
#include "SceneLoader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <string>
#include <algorithm>


USE_SDL_DATA

// Event manager for node tree event.
// Anytime a parent/child is added/switched/deleted, anytime the root object is meant to be switched,
// a signal is send out to anyone wanting to react to such events.
// 
// Uses "Object"s observer system to handle signals.
// All instances wanting to listen to node tree events must derive from "Object".

#define USE_NODE_TREE_EVENT_MANAGER extern NodeTreeEventManager* nt_event_manager;
#define NTEventManager nt_event_manager

#define USE_DEFAULT_NODE_TREE extern NodeTree* default_node_tree;
#define INodeTree default_node_tree


class Node;

typedef struct __NODE_PARENT_CHILD__{
	Node* parent;
	Node* child;
} NODE_PARENT_CHILD;

typedef union __NODE_TREE_EVENT_EX__{
	NODE_PARENT_CHILD parent_child;
	Node* node;
} NODE_TREE_EVENT_EX;

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
};

class NodeTree;

REGISTER_NODE(Node)

// Base class for all Node objects
class Node : public Object, public tse::JSONObject{
public:

	Node();
	virtual ~Node();

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
	virtual void update_global_rotation();

	uint64_t get_ID() const;
	std::string get_unique_name() const;
	bool set_unique_name(std::string name);						// assign a new unique name for the node | returns false, if name already exists
	std::string get_class_name() const;

	Node* get_parent() const;
	virtual void set_parent(Node* parent);
	// switch parent and possibly keep the global dimensions of the node
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
	std::vector<Node*> get_all_children_of_class(const std::string& class_name, bool recursive = false);		// return all children of the given class type
	
	virtual tse::JSON save() override;							// Save Node to JSON
	virtual void load(const tse::JSON& data) override;			// Load Node from JSON data

protected:
	std::string m_class;										// save class name for limiting the number necessary include directives

private:
	static uint64_t m_node_count;
	static std::set<std::string> m_used_unique_names;			// set of unique names already in use to avoid duplicats

	uint64_t m_ID;
	std::string m_unique_name;									// unique name for direct access

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

	// Use this to register a Node to the SceneLoader
	// Do this for each class derived from "Node" 
	//static tse::RegisterNode<Node> s_reg_node;

};


// NodeTree is responsible for the root object, scene switches and render setup
class NodeTree : public Object{

public:
	NodeTree(Node* root_node = nullptr){
		// assign a default root node
		if(!root_node)	m_root_node = new Node();
		else			m_root_node = root_node;
	}
	virtual ~NodeTree(){
		delete m_root_node;
	}

	// Pre-Scene setup
	void render_setup(){

	}

	Node* get_root_node(){
		return m_root_node;
	}
	void switch_root_node(Node* new_root, bool delete_irrelevant_nodes = false){
		Node* old_root = m_root_node;
		m_root_node = new_root;

		// TODO: delete nodes from old tree not in the new tree
		if(old_root && delete_irrelevant_nodes){

		}

		// Notify listeners of the root switch
		NODE_TREE_EVENT_EX ex;
		ex.node = m_root_node;
		NODE_TREE_EVENT* ev = new NODE_TREE_EVENT("switch_root", m_root_node, ex);
		emit_signal("switch_root", ev);
	}

private:
	Node* m_root_node;

};

#endif // !__NODE_H__