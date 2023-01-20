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

//tse::RegisterNode<Node> Node::s_reg_node("Node");
//static tse::RegisterNode<Node> s_reg_node("Node");

//REGISTER_NODE(Node)

uint64_t Node::m_node_count = 0;
std::set<std::string> Node::m_used_unique_names = {};

Node::Node(){
	m_ID = ++m_node_count;
	std::string name_extender = to_string(m_ID);
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



const glm::vec2& Node::get_position_2d() const{
	return glm::vec2(m_position.x, m_position.y);
}

const glm::vec3& Node::get_position() const{
	return m_position;
}
float Node::get_position_x() const{
	return m_position.x;
}
float Node::get_position_y() const{
	return m_position.y;
}
float Node::get_position_z() const{
	return m_position.z;
}
void Node::set_position_2d(const glm::vec2& position){
	m_position = glm::vec3(position.x, position.y, 0.0f);
	update_global_position();
}
void Node::set_position_2d(float x, float y){
	m_position = glm::vec3(x, y, 0.0f);
	update_global_position();
}
void Node::set_position(const glm::vec3& position){
	m_position = position;
	update_global_position();
}
void Node::set_position(float x, float y, float z){
	m_position = glm::vec3(x, y, z);
	update_global_position();
}
void Node::set_position_x(float x){
	set_position(x, m_position.y, m_position.z);
}
void Node::set_position_y(float y){
	set_position(m_position.x, y, m_position.z);
}
void Node::set_position_z(float z){
	set_position(m_position.x, m_position.y, z);
}
const glm::vec2& Node::get_global_position_2d() const{
	return glm::vec2(m_global_position.x, m_global_position.y);
}
const glm::vec3& Node::get_global_position() const{
	return m_global_position;
}
void Node::update_global_position(){
	glm::vec3 p;
	if(m_parent == NULL){
		p = glm::vec3(0.0f);
	}
	else{
		p = m_parent->m_global_position;
	}
	// set global position to parent global position + new relative position
	m_global_position = p + m_position;
	// update global positions of all children, too
	for(auto ch : m_children){
		ch->update_global_position();
	}
}
float Node::get_global_position_x() const{
	return m_global_position.x;
}
float Node::get_global_position_y() const{
	return m_global_position.y;
}
float Node::get_global_position_z() const{
	return m_global_position.z;
}

float Node::get_scale() const{
	return m_scale;
}
void Node::set_scale(float scale){
	m_scale = scale;
	update_global_scale();
}
float Node::get_global_scale() const{
	return m_global_scale;
}
void Node::update_global_scale(){
	float p = m_parent ? m_parent->m_global_scale : 1;
	m_global_scale = p * m_scale;
	for(auto ch : m_children){
		ch->update_global_scale();
	}
}

glm::vec3 Node::get_rotation() const{
	return m_rotation;
}
void Node::set_rotation(glm::vec3 rotation){
	// make sure the rotations are in (-360, 360)
	m_rotation = rotation - glm::vec3( glm::ivec3(rotation / 360.0f) * 360 );
	update_global_rotation();
}
void Node::set_rotation(float pitch, float yaw, float roll){
	set_rotation(glm::vec3(pitch, yaw, roll));
}
void Node::set_pitch(float pitch){
	set_rotation(pitch, m_rotation.y, m_rotation.z);
}
void Node::set_yaw(float yaw){
	set_rotation(m_rotation.x, yaw, m_rotation.z);
}
void Node::set_roll(float roll){
	set_rotation(m_rotation.x, m_rotation.y, roll);
}
glm::vec3 Node::get_global_rotation() const{
	return m_global_rotation;
}
void Node::update_global_rotation(){
	glm::vec3 p = m_parent ? m_parent->m_rotation : glm::vec3();
	glm::vec3 gl = p + m_rotation;
	m_global_rotation = gl - glm::vec3(glm::ivec3(gl / 360.0f) * 360);
	for(auto ch : m_children){
		ch->update_global_rotation();
	}
}


uint64_t Node::get_ID() const{
	return m_ID;
}

std::string Node::get_unique_name() const{
	return m_unique_name;
}
bool Node::set_unique_name(std::string name){
	if(m_used_unique_names.count(name)) return false;
	// remove old name from list
	m_used_unique_names.erase(m_unique_name);
	m_unique_name = name;
	// push the name to list
	m_used_unique_names.insert(m_unique_name);
	return true;
}

std::string Node::get_class_name() const{
	return m_class;
}

/* PARENT/CHILD MUTATORS */

Node* Node::get_parent() const{
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
	// call "set_parent", when you want node to have no parent
	if(!parent) return;
	// if keeping dimensions, save current global positions in tmp. vars
	glm::vec3 pos = get_global_position();
	glm::vec3 rot = get_global_rotation();
	float scale = get_global_scale();

	set_parent(parent);

	// set local dimensions to new parent minus current global dimensions, if desired
	if(keep_gl_pos)	set_position(pos.x - parent->get_global_position().x, pos.y - parent->get_global_position().y, pos.z - parent->get_global_position().z);
	if(keep_gl_rot) set_rotation(rot - parent->get_global_rotation());
	if(keep_gl_scale) set_scale(scale/parent->get_global_scale());
}

const vector<Node*>& Node::get_children() const{
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
	// update
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
void Node::delete_child_by_id(uint64_t id){

}
void Node::remove_all_children(){
	for(auto ch : m_children){
		delete ch;
	}
	m_children.clear();
}

std::vector<Node*> Node::get_all_children_of_class(const std::string& class_name, bool recursive){
	std::vector<Node*> vec = {};

	// Simple, non-recursive: Only get children of current node with given class name
	if(!recursive){
		for(Node* ch : get_children()){
			if(ch->m_class != class_name) continue;
			vec.push_back(ch);
		}
		return vec;
	}

	// Recursive: Get all nodes with given class name in subtree with current node as root 
	std::queue<Node*> q = {};
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

tse::JSON Node::save(){
	return tse::JSON();
}

void Node::load(const tse::JSON& data){

}
