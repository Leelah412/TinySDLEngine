#ifndef __NODE_H__
#define __NODE_H__

#include <SDLData.h>
#include <Object.h>

#include <SDL.h>
#include <vector>
#include <set>
#include <queue>
#include <map>
#include <string>
#include <algorithm>

using namespace std;
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

// Base class for all Node objects
class Node : public Object{
public:

	Node();
	virtual ~Node();

	virtual void input(const SDL_Event& event);			// input passed through the node tree, until a child "eats" it; return remaining event queue
	virtual void draw();								// directly draws on the screen
	virtual void update(const time_t& delta);			// updates physics n stuff

	virtual SDL_FPoint get_position();
	virtual float get_position_x();
	virtual float get_position_y();
	virtual void set_position(const SDL_FPoint& position);
	virtual void set_position(const float& x, const float& y);
	virtual void set_position_x(const float& x);
	virtual void set_position_y(const float& y);
	virtual SDL_FPoint get_global_position();
	virtual float get_global_position_x();
	virtual float get_global_position_y();
	virtual void update_global_position();

	virtual float get_scale();
	virtual void set_scale(const float& scale);
	virtual float get_global_scale();
	virtual void update_global_scale();

	virtual float get_rotation();
	virtual void set_rotation(const float& rotation);
	virtual float get_global_rotation();
	virtual void update_global_rotation();

	virtual SDL_FPoint get_size();
	virtual void set_size(const SDL_FPoint& size);
	virtual void set_size(const int& w, const int& h);

	uint64_t get_ID();
	string get_unique_name();
	bool set_unique_name(string name);						// assign a new unique name for the node | returns false, if name already exists
	string get_class_name();

	Node* get_parent();
	virtual void set_parent(Node* parent);
	// switch parent and possibly keep the global dimensions of the node
	virtual void switch_parent(Node* parent, bool keep_gl_pos = false, bool keep_gl_rot = false, bool keep_gl_scale = false);

	const vector<Node*>& get_children();
	Node* add_child(Node* child);					// add the given object as the child of the current object and return the new child
	void disown_child(Node* child);					// child object stops being child of current, but still exists
	void delete_child(Node* child);					// delete the child and its entire existence
	void delete_child_by_id(const uint64_t& id);
	void remove_all_children();						// deletes all children and their entire existence
	vector<Node*> get_all_children_of_class(string class_name, bool recursive = false);		// return all children of the given class type

protected:
	string m_class;										// save class name for limiting the number necessary include directives

private:
	static uint64_t m_node_count;
	static set<string> m_used_unique_names;				// set of unique names already in use to avoid duplicats

	uint64_t m_ID;
	string m_unique_name;								// unique name for direct access

	Node* m_parent;
	vector<Node*> m_children;

	// IMPORTANT: Do NOT allow direct access to dimensional properties, even for derived classes,
	// since with each relative change, the global dimensions also need to be altered,
	// and the calculations for them are done inside the setters!

	SDL_FPoint m_position = SDL_FPoint();				// the position of the object relative to the parent
	float m_scale = 1;									// scale the object by the given amount
	float m_rotation = 0;								// rotate the object by the given amount (in deg)
	SDL_FPoint m_size = SDL_FPoint();					// the size of the node

	// global versions relative to immovable root element
	SDL_FPoint m_global_position = SDL_FPoint();
	float m_global_scale = 1;
	float m_global_rotation = 0;
};


// NodeTree holds and is responsible for the root object and scene switches
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