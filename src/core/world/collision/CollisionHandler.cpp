#include "CollisionHandler.h"

namespace tse{

// since we add listener to node tree event manager in the constructor, collision handler must be inited manually
//CollisionHandler* collision_handler = nullptr;
CollisionHandler* CollisionHandler::s_default_collision_handler = nullptr;


CollisionHandler::CollisionHandler(){
	// listen to parent/child mutation events from physics objects
	NTEventManager->listen_child_added_event(this);
	NTEventManager->listen_child_removed_event(this);
	NTEventManager->listen_node_to_delete_event(this);
	NTEventManager->listen_switch_root_event(this);
}
CollisionHandler::~CollisionHandler(){
	NTEventManager->drop_child_added_event(this);
	NTEventManager->drop_child_removed_event(this);
	NTEventManager->drop_node_to_delete_event(this);
	NTEventManager->drop_switch_root_event(this);
}

void CollisionHandler::update(time_t delta){

}

void CollisionHandler::add_object(PhysicsObject* obj){

}
void CollisionHandler::remove_object(PhysicsObject* obj){

}

void CollisionHandler::update_sweep_list(){
	// update static
	for(map<float, set<PhysicsObject*>>::iterator it = m_sweep_static.begin(); it != m_sweep_static.end(); it++){

	}
}

//multimap<float, PhysicsObject*> CollisionHandler::get_sweep_list(){
//	return {};
//}
multiset<Collision> CollisionHandler::get_active_collisions(){
	return m_active_collisions;
}

// CURRENTLY, we are only sweeping x axis, but maybe change it later to that one variance method
void CollisionHandler::sweep(time_t delta){

	// no need to check for collisions, if there are no moving objects
	if(m_dynamic_objects.empty()) return;

	/* Extend axis reach of currently moving dynamic objects temporarily by x velocity to check collisions */
	sweep_helper_assign_velocity(delta);
	/* Sweep both lists */
	sweep_helper_sweep();

	// Next:
	// 
	// 1.	Discard collisions with collision time 0 (already colliding at start of frame). Maybe do that while sweeping.
	// 2.	Sort collision list by the time of occurrence of the collision.
	// 3.	For each earliest collision:
	//		a.	Call "handle_collision" function for colliding objects.
	//		b1.	Overwritten function handling collision alone for one object:
	//			Update dynamic sweep list with new positions excluding velocity for that object,
	//			only handle the other object from 3c onwards.
	//		b2.	Overwritten function handling collision alone for both:
	//			Update dynamic sweep list with new positions excluding velocity, then jump to 3.
	//		c.	Update positions in the sweep list including remaining velocity.
	//		d.	Sweep the sweep lists beginning after and ending before the start/end points
	//			incl. velocity of dynamic objects to check for next collisions.
	//		e1.	Found collisions: Push newly found collisions to collision list, then jump to 2.
	//		e2.	Didn't find collision: Move objects by remaining velocity until delta,
	//			update dynamic sweep list with new positions excluding velocity, then jump to 3.

	/* For each collision, call the "handle_collision" function from each object */

	/* Update dynamic sweep list with new positions */

}
void CollisionHandler::narrow_detection(PhysicsObject* obj1, PhysicsObject* obj2){

}

void CollisionHandler::sweep_helper_assign_velocity(time_t delta){
	// no need to check for collisions, if there are no moving objects
	if(m_dynamic_objects.empty()) return;


	/* Extend axis reach of currently moving dynamic objects temporarily by x velocity to check collisions */

	float vel_x, pos_x, start, end;
	map<float, set<DynamicObject*>>::iterator it_sweep;
	map<float, set<PhysicsObject*>>::iterator it_sweep_st;
	for(set<DynamicObject*>::iterator it = m_dynamic_objects.begin(); it != m_dynamic_objects.end(); it++){
		// currently only need to check x velocity
		if((vel_x = (*it)->get_velocity().x) == 0.0f) continue;
		// collision shape must exist
		if((*it)->get_collision_shape()){
			cerr << "ERROR: dynamic object has no collision shape" << endl;
			continue;
		}

		// moving left -> only need to update start value
		start = (*it)->get_global_position_x() + (*it)->get_collision_shape()->get_aabb().x;
		end = start + (*it)->get_collision_shape()->get_aabb().w;
		// moving right -> only need to update end value
		if(vel_x < 0)	pos_x = start;
		else			pos_x = end;

		it_sweep = m_sweep_dynamic.find(pos_x);
		if(it_sweep == m_sweep_dynamic.end()){
			cerr << "ERROR: global position + aabb position of dynamic object not in map" << endl;
			continue;
		}
		// remove old start/end value
		(*it_sweep).second.erase(*it);
		if((*it_sweep).second.empty()){
			m_sweep_dynamic.erase(it_sweep);
		}
		// insert new start/end value
		if((it_sweep = m_sweep_dynamic.find(pos_x + vel_x)) != m_sweep_dynamic.end()){
			(*it_sweep).second.insert(*it);
		}
		else{
			m_sweep_dynamic.insert(pair<float, set<DynamicObject*>>(pos_x + vel_x, {*it}));
		}

	}
}
void CollisionHandler::sweep_helper_sweep(){
	// list of active objects
	set<PhysicsObject*> st_active = {};
	set<DynamicObject*> dy_active = {};
	// list of active collisions
	multimap<DynamicObject*, PhysicsObject*> dy_st_cols = {};
	multimap<DynamicObject*, DynamicObject*> dy_dy_cols = {};

	map<float, set<DynamicObject*>>::iterator it_sweep;
	map<float, set<PhysicsObject*>>::iterator it_sweep_st;

	it_sweep = m_sweep_dynamic.begin();
	it_sweep_st = m_sweep_static.begin();

	// TODO: it's possible, that an object(s collision shape) starts AND ends at the same position on the axis (1D object),
	// so handle that accordingly

	// We only want to sweep both lists, until either one of them ends
	while((it_sweep != m_sweep_dynamic.end()) && (it_sweep_st != m_sweep_static.end())){
		// activate dynamic instance(s)
		if((*it_sweep).first < (*it_sweep_st).first){
			// TODO: for two objects on the exact same x axis, it could be possible, that sometimes a collision will be detected, and sometimes not,
			// depending on order of the check, so think about that 
			for(set<DynamicObject*>::iterator it = (*it_sweep).second.begin(); it != (*it_sweep).second.end(); it++){
				if(!dy_active.count(*it)){
					time_t tmp;
					// check collision of dynamic object with all active dynamic and static instances
					for(set<DynamicObject*>::iterator it2 = dy_active.begin(); it2 != dy_active.end(); it2++){
						if((*it)->move_and_collide(*it, *it2, 0, tmp)){
							dy_dy_cols.insert(pair<DynamicObject*, DynamicObject*>(*it, *it2));
						}
					}
					for(set<PhysicsObject*>::iterator it2 = st_active.begin(); it2 != st_active.end(); it2++){
						if((*it)->move_and_collide(*it, *it2, 0, tmp)){
							dy_st_cols.insert(pair<DynamicObject*, PhysicsObject*>(*it, *it2));
						}

					}

					dy_active.insert(*it);
				}
				else
					dy_active.erase(*it);
			}
			it_sweep++;
		}
		// activate static instance(s)
		else if((*it_sweep).first > (*it_sweep_st).first){
			for(set<PhysicsObject*>::iterator it = (*it_sweep_st).second.begin(); it != (*it_sweep_st).second.end(); it++){
				if(!st_active.count(*it)){
					// check collision of static object with all active dynamic instances

					st_active.insert(*it);
				}
				else
					st_active.erase(*it);
			}

			it_sweep_st++;
		}
		// active instances of both types
		else{
			for(set<DynamicObject*>::iterator it = (*it_sweep).second.begin(); it != (*it_sweep).second.end(); it++){
				if(!dy_active.count(*it)){
					// check collision of dynamic object with all active dynamic and static instances

					dy_active.insert(*it);
				}
				else
					dy_active.erase(*it);
			}
			for(set<PhysicsObject*>::iterator it = (*it_sweep_st).second.begin(); it != (*it_sweep_st).second.end(); it++){
				if(!st_active.count(*it)){
					// check collision of static object with all active dynamic instances

					st_active.insert(*it);
				}
				else
					st_active.erase(*it);
			}

			it_sweep++;
			it_sweep_st++;
		}
	}

	// Sweep remaining dynamic objects, if they exists
	// No need to sweep remaining static objects, since they can't do anything anyway
	if(it_sweep != m_sweep_dynamic.end()){
		for(set<DynamicObject*>::iterator it = (*it_sweep).second.begin(); it != (*it_sweep).second.end(); it++){
			if(!dy_active.count(*it)){
				// check collision of dynamic object with all active dynamic and static instances

				dy_active.insert(*it);
			}
			else
				dy_active.erase(*it);
		}
		it_sweep++;
	}
}



void CollisionHandler::update_on_signal(SIGNAL signal, EVENT* event){
	// add child
	NODE_TREE_EVENT* ev;
	PhysicsObject* po;
	if(event->signal == "child_added"){
		if(!(ev = dynamic_cast<NODE_TREE_EVENT*>(event))) return;
		if(!(po = dynamic_cast<PhysicsObject*>(ev->ex.parent_child.child))) return;
		cout << "child " << po->get_unique_name() << " added" << endl;
	}
	else if(event->signal == "child_delete"){

	}
}

CollisionHandler* CollisionHandler::get_default_collision_handler(){
	return s_default_collision_handler;
}

void CollisionHandler::set_default_collision_handler(CollisionHandler* handler){
	s_default_collision_handler = handler;
}

}

