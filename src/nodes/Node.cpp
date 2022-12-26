#include "Node.h"


// NodeTreeEventManager


NodeTreeEventManager* nt_event_manager = new NodeTreeEventManager();

NodeTreeEventManager::NodeTreeEventManager(){

}
NodeTreeEventManager::~NodeTreeEventManager(){

}

// NOTIFICATIONS
void NodeTreeEventManager::child_added(Node* parent, Node* child){
	NODE_TREE_EVENT_EX ex;
	ex.parent_child = NODE_PARENT_CHILD();
	ex.parent_child.parent = parent;
	ex.parent_child.child = child;

	// check, if parent and child are actually related
	if(child->get_parent() != parent) return;
	if(find(parent->get_children().begin(), parent->get_children().end(), child) == parent->get_children().end()) return;

	// check, if nodes are part of the current tree (i.e. parent sequence leads to root object)
	Node* it = parent;
	while(it->get_parent()){
		it = it->get_parent();
	}
	//if(it != )

	emit_signal("child_added", new NODE_TREE_EVENT("child_added", nullptr, ex));

}
void NodeTreeEventManager::child_removed(Node* parent, Node* child){
	NODE_TREE_EVENT_EX ex;
	ex.parent_child = NODE_PARENT_CHILD();
	ex.parent_child.parent = parent;
	ex.parent_child.child = child;

	emit_signal("child_removed", new NODE_TREE_EVENT("child_removed", nullptr, ex));

}
void NodeTreeEventManager::node_to_delete(Node* node){
	NODE_TREE_EVENT_EX ex;
	ex.node = node;
	emit_signal("node_to_delete", new NODE_TREE_EVENT("node_to_delete", nullptr, ex));
}

// COMMANDS
void NodeTreeEventManager::switch_root(Node* new_root){
	NODE_TREE_EVENT_EX ex;
	ex.node = new_root;
	emit_signal("switch_root", new NODE_TREE_EVENT("switch_root", nullptr, ex));
}

// LISTENERS
void NodeTreeEventManager::listen_child_added_event(Object* listener){
	add_listener("child_added", listener);
}
void NodeTreeEventManager::listen_child_removed_event(Object* listener){
	add_listener("child_removed", listener);
}
void NodeTreeEventManager::listen_node_to_delete_event(Object* listener){
	add_listener("node_to_delete", listener);
}
void NodeTreeEventManager::listen_switch_root_event(Object* listener){
	add_listener("switch_root", listener);
}

void NodeTreeEventManager::drop_child_added_event(Object* listener){
	remove_listener("child_added", listener);
}
void NodeTreeEventManager::drop_child_removed_event(Object* listener){
	remove_listener("child_removed", listener);
}
void NodeTreeEventManager::drop_node_to_delete_event(Object* listener){
	remove_listener("node_to_delete", listener);
}
void NodeTreeEventManager::drop_switch_root_event(Object* listener){
	remove_listener("switch_root", listener);
}


// NodeTree


NodeTree* default_node_tree = nullptr;


// Node


uint64_t Node::m_node_count = 0;
set<string> Node::m_used_unique_names = {};

Node::Node(){
	m_ID = ++m_node_count;
	string name_extender = to_string(m_ID);
	while(!set_unique_name("node_" + name_extender)){
		// just add the id multiple times, if default name is given somehow
		name_extender += to_string(m_ID);
	}
	cout << "node with unique name " << m_unique_name << " initialized" << endl;
	m_parent = NULL;
	m_children = {};
	m_class = "Node";
}

Node::~Node(){
	// remove name from unique name list!
	m_used_unique_names.erase(m_unique_name);
	
	// call deconstructor of all children
	for(auto ch : m_children){
		delete ch;
	}
}

void Node::input(const SDL_Event& event){

}

void Node::draw(){

}

void Node::update(const time_t& delta){
	
}



SDL_FPoint Node::get_position(){
	return m_position;
}
float Node::get_position_x(){
	return m_position.x;
}
float Node::get_position_y(){
	return m_position.y;
}
void Node::set_position(const SDL_FPoint& position){
	set_position(position.x, position.y);
}
void Node::set_position(const float& x, const float& y){
	SDL_FPoint p;
	p.x = x;
	p.y = y;
	m_position = p;
	update_global_position();
}
void Node::set_position_x(const float& x){
	set_position(x, m_position.y);
}
void Node::set_position_y(const float& y){
	set_position(m_position.x, y);
}
SDL_FPoint Node::get_global_position(){
	return m_global_position;
}
void Node::update_global_position(){
	SDL_FPoint p;
	if(m_parent == NULL){
		p.x = 0;
		p.y = 0;
	}
	else{
		p = m_parent->m_global_position;
	}
	// set global position to parent global position + new relative position
	m_global_position.x = p.x + m_position.x;
	m_global_position.y = p.y + m_position.y;
	// update global positions of all children, too
	for(auto ch : m_children){
		ch->update_global_position();
	}
}
float Node::get_global_position_x(){
	return m_global_position.x;
}
float Node::get_global_position_y(){
	return m_global_position.y;
}

float Node::get_scale(){
	return m_scale;
}
void Node::set_scale(const float& scale){
	m_scale = scale;
	update_global_scale();
}
float Node::get_global_scale(){
	return m_global_scale;
}
void Node::update_global_scale(){
	float p = m_parent ? m_parent->m_global_scale : 1;
	m_global_scale = p * m_scale;
	for(auto ch : m_children){
		ch->update_global_scale();
	}
}

float Node::get_rotation(){
	return m_rotation;
}
void Node::set_rotation(const float& rotation){
	// make sure the rotations are in (-360, 360)
	m_rotation = rotation - ( int(rotation / 360) * 360 );
	update_global_rotation();
}
float Node::get_global_rotation(){
	return m_global_rotation;
}
void Node::update_global_rotation(){
	float p = m_parent ? m_parent->m_rotation : 0;
	float gl = p + m_rotation;
	m_global_rotation = gl - (int(gl / 360) * 360);
	for(auto ch : m_children){
		ch->update_global_rotation();
	}
}


SDL_FPoint Node::get_size(){
	return m_size;
}

void Node::set_size(const SDL_FPoint& size){
	m_size = size;
}

void Node::set_size(const int& w, const int& h){
	m_size.x = w;
	m_size.y = h;
}

uint64_t Node::get_ID(){
	return m_ID;
}

string Node::get_unique_name(){
	return m_unique_name;
}
bool Node::set_unique_name(string name){
	if(m_used_unique_names.count(name)) return false;
	// remove old name from list
	m_used_unique_names.erase(m_unique_name);
	m_unique_name = name;
	// push the name to list
	m_used_unique_names.insert(m_unique_name);
	return true;
}

string Node::get_class_name(){
	return m_class;
}

/* PARENT/CHILD MUTATORS */

Node* Node::get_parent(){
	return m_parent;
}

// TODO: some things might become problematic, if switching to given parent causes a circle in the tree
void Node::set_parent(Node* parent){
	if(parent == m_parent) return;
	// first remove node from current parent, if needed
	if(m_parent){
		// if current parent doesn't contain child anymore, it means child has initiated adoption
		// per set_parent() already and we can return to avoid stack overflow
		//cout << "looking for self in parent list" << endl;
		if(find(m_parent->get_children().begin(), m_parent->get_children().end(), this) == m_parent->get_children().end()) return;
		//cout << "self parent list" << endl;
		m_parent->disown_child(this);
		//cout << "self disowned" << endl;
	}
	m_parent = parent;
	// parent set to NULL
	if(!m_parent) return;
	//cout << "trying to get a new parent" << endl;
	// check, if parent already contains current node, otherwise add this to parent
	if(find(m_parent->get_children().begin(), m_parent->get_children().end(), this) != m_parent->get_children().end()) return;
	m_parent->add_child(this);
	//cout << "got a new parent" << endl;
}

void Node::switch_parent(Node* parent, bool keep_gl_pos, bool keep_gl_rot, bool keep_gl_scale){
	// if keeping dimensions, save current global positions in tmp. vars
	SDL_FPoint pos = get_global_position();
	float rot = get_global_rotation();
	float scale = get_global_scale();

	set_parent(parent);

	// set local dimensions to new parent minus current global dimensions, if desired
	if(keep_gl_pos)	set_position(pos.x - parent->get_global_position().x, pos.y - parent->get_global_position().y);
	if(keep_gl_rot) set_rotation(rot - parent->get_global_rotation());
	if(keep_gl_scale) set_scale(scale/parent->get_global_scale());
}

const vector<Node*>& Node::get_children(){
	return m_children;
}
Node* Node::add_child(Node* child){
	if(!child) return NULL;
	m_children.push_back(child);
	// parent of child is already current node
	if(child->get_parent() == this){
		// emit signal, that a child has been added to the parent
		NTEventManager->child_added(this, child);
		return child;
	}
	child->set_parent(this);
	NTEventManager->child_added(this, child);
	return child;
}
void Node::disown_child(Node* child){
	if(!child) return;
	vector<Node*>::iterator it = find(m_children.begin(), m_children.end(), child);
	if(it == m_children.end()) return;
	//cout << "disown" << endl;
	m_children.erase(it);
	NTEventManager->child_removed(this, child);
	//cout << "disowndisown" << endl;
	if(!child->get_parent()) return;					// child has already left parent
	//cout << "setting childs parent to null" << endl;
	child->set_parent(NULL);
}
void Node::delete_child(Node* child){
	if(!child) return;
	// remove child from children list first!
	disown_child(child);
	// signal, that child is ABOUT TO BE deleted (NOT that it's already deleted!)
	NTEventManager->node_to_delete(child);
	delete child;
}
void Node::delete_child_by_id(const uint64_t& id){

}
void Node::remove_all_children(){
	for(auto ch : m_children){
		delete ch;
	}
	m_children.clear();
}

vector<Node*> Node::get_all_children_of_class(string class_name, bool recursive){
	vector<Node*> vec = {};

	// Simple, non-recursive: Only get children of current node with given class name
	if(!recursive){
		for(Node* ch : get_children()){
			if(ch->m_class != class_name) continue;
			vec.push_back(ch);
		}
		return vec;
	}

	// Recursive: Get all nodes with given class name in subtree with current node as root 
	queue<Node*> q = {};
	Node* front;
	q.push(this);
	while(!q.empty()){
		front = q.front();
		q.pop();
		for(Node* ch : front->get_children()){
			q.push(ch);
		}
		if(front->m_class != class_name) continue;
		vec.push_back(front);
	}

	return vec;
}