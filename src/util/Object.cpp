#include "Object.h"

Object::Object(){
	m_signal_observers = {};
}
Object::~Object(){}

void Object::emit_signal(SIGNAL signal, EVENT* event){
	if(m_signal_observers.find(signal) == m_signal_observers.end())	return;

	for(auto obs : m_signal_observers[signal]){
		obs->update_on_signal(signal, event);
	}
}
void Object::update_on_signal(SIGNAL signal, EVENT* event){
	// nothing to do for basic Node instance
}

void Object::add_listener(SIGNAL signal, Object* observer){
	// add signal to list, if it doesnt exist
	if(m_signal_observers.find(signal) == m_signal_observers.end()){
		m_signal_observers.insert(pair<SIGNAL, OBSERVER_LIST>(signal, {}));
	}

	m_signal_observers[signal].insert(observer);
}
void Object::remove_listener(SIGNAL signal, Object* observer){
	// if signal is not in the list, it means object is definitely not there; unnecessary call, return
	if(m_signal_observers.find(signal) == m_signal_observers.end())	return;

	m_signal_observers[signal].erase(observer);

	// remove signal from list, if we just erased its last observer
	if(m_signal_observers[signal].empty()){
		m_signal_observers.erase(signal);
	}
}
