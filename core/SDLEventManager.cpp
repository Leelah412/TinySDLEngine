#include "SDLEventManager.h"

SDLEventManager* sdl_event_manager = new SDLEventManager();


SDLEventManager::SDLEventManager(){}
SDLEventManager::~SDLEventManager(){}


bool SDLEventManager::add_event_listener(SEM_EVENT event, SEMObject* listener){
	return add_event_listener(event, SEM_EVENT_EXT(), listener);
}

bool SDLEventManager::add_event_listener(SEM_EVENT event, SEM_EVENT_EXT ext, SEMObject* listener){
	if(listener == NULL) return false;

	// event isn't in the event list
	if(!m_listeners.count(event)){

		m_listeners.insert( pair<SEM_EVENT, map<SEM_EVENT_EXT, set<SEMObject*>>>(event, { {ext, {listener}} }) );
		cout << "added new event, ext and listener" << endl;
		return true;
	}

#if DEBUG_MODE
	// since this does the same as the condition below, this isn't relevant for the execution, but could be important for debugging purposes
	if(m_listeners[event].find(ext) == m_listeners[event].end()){
		m_listeners[event][ext].insert(listener);
		cout << "added extension and listener to event" << endl;
		return true;
	}
#endif // DEBUG_MODE

	if(m_listeners[event][ext].find(listener) != m_listeners[event][ext].end()){
		cout << "WARNING: attempt to insert duplicate entry" << endl;
		return false;
	}

	m_listeners[event][ext].insert(listener);
	cout << "added listener to event and extension" << endl;
	return true;
}


bool SDLEventManager::remove_event_listener(SEM_EVENT event, SEMObject* listener, bool keep_non_null_ext){
	// Event doesn't exist
	if(!m_listeners.count(event)) return false;

	// Only remove listeners with extended type "SEM_NULL"
	if(keep_non_null_ext){

		SEM_EVENT_EXT _ext = SEM_EVENT_EXT();
		// No null extension exists for this event
		if(!m_listeners[event].count(_ext)) return false;
		// Event and extension exist -> erase the listener from the list
		// Return false, if listener never listened to event
		if(!m_listeners[event][_ext].erase(listener)) return false;

		// No need to keep events no one listens to in the list
		if(!m_listeners[event][_ext].empty()) return true;
		m_listeners[event].erase(_ext);
		if(!m_listeners[event].empty()) return true;
		m_listeners.erase(event);

		return true;
	}

	// Remove ALL attachments to type "event" for listener

	bool erased = false;
	set<SEMObject*>::iterator it;

	for(auto ext : m_listeners[event]){
		// Remove listener, if it exists, otherwise continue
		if( !(erased |= ext.second.erase(listener)) ) continue;
		// Remove unlistened events
		if(!ext.second.empty()) continue;
		m_listeners[event].erase(ext.first);
	}

	// Listener list empty check
	if(m_listeners[event].empty()) m_listeners.erase(event);

	return erased;
}

bool SDLEventManager::remove_event_listener(SEM_EVENT event, SEM_EVENT_EXT ext, SEMObject* listener){
	// Event doesn't exist
	if(!m_listeners.count(event)) return false;
	// Extension doesn't exist for this event
	if(!m_listeners[event].count(ext)) return false;

	// Erase the listener from the list
	// Return false, if listener never listened to event
	if(!m_listeners[event][ext].erase(listener)) return false;

	// No need to keep events no one listens to in the list
	if(!m_listeners[event][ext].empty()) return true;
	m_listeners[event].erase(ext);
	if(!m_listeners[event].empty()) return true;
	m_listeners.erase(event);

	return true;
}


vector<pair<SEM_EVENT, SEM_EVENT_EXT>> SDLEventManager::get_listened_events(SEMObject* listener){
	return {};
}


void SDLEventManager::poll_next_event(){
	SDL_Event ev;
	if(SDL_PollEvent(&ev))
		handle_event(ev);
}
void SDLEventManager::poll_events(){
	SDL_Event ev;
	while(SDL_PollEvent(&ev)) handle_event(ev);
}


void SDLEventManager::start(){

}
void SDLEventManager::stop(){

}

void SDLEventManager::loop(){

}


void SDLEventManager::notify(SEM_EVENT event, SEM_EVENT_EXT ext, const SDL_Event& sdl_event){
	//cout << "notifying listeners" << endl;
	auto lst = m_listeners[event];
	map<SEM_EVENT_EXT, set<SEMObject*>>::iterator it;
	SEM_EVENT_EXT cor_ext;

	// find the correct extended event entry
	for(it = lst.begin(); it != lst.end(); it++){
		cor_ext = (*it).first;

		// functions for events without extensions are called no matter the extension ...
		if(cor_ext.type == SEMEX_NULL){
			for(auto obj : (*it).second){
				//cout << "base event handler called" << endl;
				obj->handle_event(sdl_event, event, ext);
			}
			// fired event has no extension in the first place -> all necessary functions have been executed already, so stop
			if(ext.type == SEMEX_NULL) break;
			// otherwise go on with the next extension
			continue;
		}
		// ... for every other event, the extensions must be equivalent
		if(cor_ext != ext) continue;

		// found the correct extension
		// execute all functions for this event
		for(auto obj : (*it).second){
			//cout << "extended event handler called" << endl;
			obj->handle_event(sdl_event, event, ext);
		}

		break;
	}
}

void SDLEventManager::notify_helper(const SDL_Event& event){
	SEM_EVENT_EXT_UNION ext = SEM_EVENT_EXT_UNION();
	switch(event.type){
		case SDL_QUIT:
			notify(SEM_QUIT, SEM_EVENT_EXT(), event);
			break;

			// For key and mouse button events, we always pass a non-null extended event
		case SDL_KEYDOWN:
			ext.keycode = event.key.keysym.sym;
			notify(SEM_KEY_DOWN, SEM_EVENT_EXT(SEMEX_KEYCODE, ext), event);
			ext.scancode = event.key.keysym.scancode;
			notify(SEM_KEY_DOWN, SEM_EVENT_EXT(SEMEX_SCANCODE, ext), event);
			break;

		case SDL_KEYUP:
			ext.keycode = event.key.keysym.sym;
			notify(SEM_KEY_UP, SEM_EVENT_EXT(SEMEX_KEYCODE, ext), event);
			notify(SEM_KEY_PRESSED, SEM_EVENT_EXT(SEMEX_KEYCODE, ext), event);
			notify(SEM_ANY_PRESSED, SEM_EVENT_EXT(SEMEX_KEYCODE, ext), event);

			ext.scancode = event.key.keysym.scancode;
			notify(SEM_KEY_UP, SEM_EVENT_EXT(SEMEX_SCANCODE, ext), event);
			notify(SEM_KEY_PRESSED, SEM_EVENT_EXT(SEMEX_SCANCODE, ext), event);
			notify(SEM_ANY_PRESSED, SEM_EVENT_EXT(SEMEX_SCANCODE, ext), event);
			break;

		case SDL_MOUSEBUTTONDOWN:
			ext.mousebutton = event.button.button;
			notify(SEM_MOUSE_DOWN, SEM_EVENT_EXT(SEMEX_MOUSEBUTTON, ext), event);
			break;

		case SDL_MOUSEBUTTONUP:
			ext.mousebutton = event.button.button;
			notify(SEM_MOUSE_UP, SEM_EVENT_EXT(SEMEX_MOUSEBUTTON, ext), event);
			notify(SEM_MOUSE_PRESSED, SEM_EVENT_EXT(SEMEX_MOUSEBUTTON, ext), event);
			notify(SEM_ANY_PRESSED, SEM_EVENT_EXT(SEMEX_MOUSEBUTTON, ext), event);
			break;
		case SDL_MOUSEMOTION:
			ext.mousebutton = event.button.button;
			notify(SEM_MOUSE_MOVE, SEM_EVENT_EXT(), event);
			break;

		default: break;
	}
}

void SDLEventManager::handle_event(const SDL_Event& event){
	notify_helper(event);
}



//////////////////
//				//
//	SEMObject	//
//				//
//////////////////

SEMObject::SEMObject(){}
SEMObject::~SEMObject(){}


void SEMObject::handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext){}


int SEMObject::listen_event(SEM_EVENT event){
	return (int) EventManager->add_event_listener(event, this);
}

int SEMObject::listen_event(SEM_EVENT event, SEM_EVENT_EXT ext){
	return (int) EventManager->add_event_listener(event, ext, this);

}

int SEMObject::listen_event(vector<pair<SEM_EVENT, SEM_EVENT_EXT>> event_list){
	int fail_count = 0;
	for(auto ev : event_list){
		fail_count += (int) EventManager->add_event_listener(ev.first, ev.second, this);
	}
	return fail_count;
}


int SEMObject::drop_event(SEM_EVENT event){
	return (int)EventManager->remove_event_listener(event, this);
}

int SEMObject::drop_event(SEM_EVENT event, SEM_EVENT_EXT ext){
	return (int)EventManager->remove_event_listener(event, ext, this);
}

int SEMObject::drop_event(vector<pair<SEM_EVENT, SEM_EVENT_EXT>> event_list){
	int fail_count = 0;
	for(auto ev : event_list){
		fail_count += (int)EventManager->remove_event_listener(ev.first, ev.second, this);
	}
	return fail_count;
}

int SEMObject::drop_all_events(){
	return 0;
}
