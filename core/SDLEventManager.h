#ifndef __INPUT_H__
#define __INPUT_H__

#define DEBUG_MODE 1

#include <SDL.h>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <string>
#include <memory>
#include <iostream>

//////////////////////////////////////////
//										//
//	A simple handler for SDL events.	//
//										//
//////////////////////////////////////////

#define USE_EVENT_MANAGER extern SDLEventManager* sdl_event_manager;	// Write this after the include/define directives to be able to use the SDL event manager
#define EventManager sdl_event_manager									// Define "EventManager" for a simpler access to the default instance

using namespace std;

typedef deque<SDL_Event> SDL_EVENT_LIST;								// List of SDL events

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//	Events are differentiated between basic and extended events:							//
//																							//
//	- basic:	No extra information required to determine the correct use-case				//
//	- extended:	Need more information to correctly determine the use-case					//
//																							//
//	Ex. Basic:	Instance wants to be notified anytime any keyboard key has been pressed.	//
//				Firing an "ANY_KEY_PRESSED" event is enough.								//
//	Ex. Exten.: Instance wants to be notified anytime the shift key has been let go.		//
//				Besides a "KEY_UP" event, we also need to track the correct key to			//
//				ensure, that we don't just waste											//
//																							//
//	In theory, since we also pass the corresponding SDL_Event, basic event tracking is		//
//	sufficient in any use case, but instances tracking e.g. key events would have to		//
//	manually determine (with the passed SDL_Event), whether the correct key was pressed.	//
//	Therefore, extended events are useful to lower the number of function calls,			//
//	since a key event would only fire for those instances, which track that exact key.		//
//																							//
//	The following events are currently supported:											//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

typedef enum __SEM_EVENT__ {

	/* BASIC EVENTS */
	SEM_QUIT,								// SDL_Quit event
	SEM_MOUSE_MOVE,							// The mouse has been moved
	SEM_KEYMAP_CHANGED,						// The keymap has been changed
	SEM_ANY_PRESSED,						// ANYTHING, that's clickable, has been pressed (key, mousebutton, joystick...) | Can also be extended

	/* KEYBOARD EVENTS */
	/* RELEVANT EVENT EXTENSION VARIABLES: keycode, scancode */
	SEM_KEY_PRESSED,						// Key has been pressed (i.e. held down and let go) | Fired at the same time as "KEY_UP" event
	SEM_KEY_DOWN,							// Key is being held down
	SEM_KEY_UP,								// Key has been let go | Fired at the same time as "KEY_PRESSED" event
	SEM_TEXTINPUT,							// Key has been pressed for writing text (in most cases, this is most useful as a basic event)
	SEM_TEXTEDITING,						// Simmilar as above

	/* MOUSEBUTTON EVENTS */
	/* RELEVANT EVENT EXTENSION VARIABLES: mousebutton */
	SEM_MOUSE_PRESSED,						// Similar to "KEY_PRESSED" for mouse buttons
	SEM_MOUSE_DOWN,							// Similar to "KEY_DOWN" for mouse buttons
	SEM_MOUSE_UP,							// Similar to "KEY_UP" for mouse buttons

} SEM_EVENT;


// EXTENDED EVENTS

typedef enum __SEM_EVENT_EXT_TYPE__ { SEMEX_NULL = 0, SEMEX_KEYCODE, SEMEX_SCANCODE, SEMEX_MOUSEBUTTON } SEM_EVENT_EXT_TYPE;

// Each variable represents one possible extension at once
typedef union {
	SDL_Keycode keycode;
	SDL_Scancode scancode;
	Uint8 mousebutton;				// TODO: check, whether there is an SDL-native mousebutton enum, otherwise implement own
} SEM_EVENT_EXT_UNION;

// Event extension struct
typedef struct __SEM_EVENT_EXT__{

	SEM_EVENT_EXT_TYPE type;
	SEM_EVENT_EXT_UNION event;

	// Default: no event extension
	__SEM_EVENT_EXT__(){
		type = SEMEX_NULL;
	}
	__SEM_EVENT_EXT__(SEM_EVENT_EXT_TYPE _type, SEM_EVENT_EXT_UNION _event){
		type = _type;
		event = _event;
	}
	__SEM_EVENT_EXT__(SDL_Keycode _keycode){
		type = SEMEX_KEYCODE;
		event.keycode = _keycode;
	}
	__SEM_EVENT_EXT__(SDL_Scancode _scancode){
		type = SEMEX_SCANCODE;
		event.scancode = _scancode;
	}
	__SEM_EVENT_EXT__(Uint8 _mousebutton){
		type = SEMEX_MOUSEBUTTON;
		event.mousebutton = _mousebutton;
	}

	bool operator==(const __SEM_EVENT_EXT__& ext){
		if(type != ext.type) return false;
		if(type == SEMEX_KEYCODE && event.keycode != ext.event.keycode) return false;
		if(type == SEMEX_SCANCODE && event.scancode != ext.event.scancode) return false;
		if(type == SEMEX_MOUSEBUTTON && event.mousebutton != ext.event.mousebutton) return false;

		return true;
	}
	bool operator!=(const __SEM_EVENT_EXT__& ext){
		return !(*this == ext);
	}

	bool operator<(const __SEM_EVENT_EXT__& ext) const{
		// Type has highest priority
		if(type != ext.type) return type < ext.type;
		switch(type){
			case SEMEX_KEYCODE: return event.keycode < ext.event.keycode;
			case SEMEX_SCANCODE: return event.scancode < ext.event.scancode;
			case SEMEX_MOUSEBUTTON: return event.mousebutton < ext.event.mousebutton;
			default: return false;
		}
		return false;
	}

} SEM_EVENT_EXT;


class SEMObject;
class SDLEventManager{

public:
	SDLEventManager();
	virtual ~SDLEventManager();

	// Start listening to the given event.
	// Returns false, if operation is invalid (e.g. listener already listens to given event).
	virtual bool add_event_listener(SEM_EVENT event, SEMObject* listener);
	// Call this to add extended events!
	// Returns false, if operation is invalid (e.g. extended event has no relevance for given event).
	virtual bool add_event_listener(SEM_EVENT event, SEM_EVENT_EXT ext, SEMObject* listener);


	// Stop listening to the given event.
	// Returns false, if operation is invalid.
	// If "keep_non_null_ext", only non-extended events will be removed,
	// otherwise ALL events of the given kind will be removed for the given listener!
	virtual bool remove_event_listener(SEM_EVENT event, SEMObject* listener, bool keep_non_null_ext = true);
	// Call this to remove extended events!
	virtual bool remove_event_listener(SEM_EVENT event, SEM_EVENT_EXT ext, SEMObject* listener);

	// Return all events listened to by the given object
	// TODO: make a list tracking all listened events of an object
	virtual vector<pair<SEM_EVENT, SEM_EVENT_EXT>> get_listened_events(SEMObject* listener);

	/* ONLY CALL THE POLL EVENTS FROM ANOTHER INSTANCE, IF POLLING IS SUPPOSED TO BE DONE EXTERNALLY */

	// Handles the next event in the SDL event queue
	virtual void poll_next_event();
	// Handles all events currently in the SDL event queue
	virtual void poll_events();


	/* CALL start() AND stop() ONLY, IF */

	// Start the event tracking
	// STARTS SEPERATE THREAD | ONLY CALL, IF YOUR APPLICATION CAN HANDLE FUNCTION CALLS FROM DIFFERENT THREADS
	virtual void start();
	// Stop the event tracking, also stops thread
	virtual void stop();

protected:
	// Handle the given event and possibly notify listeners
	virtual void handle_event(const SDL_Event& event);
	// Fire the events and notify listeners
	virtual void notify(SEM_EVENT event, SEM_EVENT_EXT ext, SDL_Event sdl_event);
	// Helper function to correctly identify the listeners of a given event
	virtual void notify_helper(const SDL_Event& event);

private:

	// If wished, SDLEventManager can fetch SDL events in a loop in its own thread, therefore able to fire immediately
	bool m_run = false;
	map<SEM_EVENT, map<SEM_EVENT_EXT, set<SEMObject*>>> m_listeners = {};

	// The loop executed in a seperate thread, if "m_run" is true
	virtual void loop();

};

class SEMObject{

public:
	SEMObject();
	virtual ~SEMObject();

	// Called by SDLEventManager, when notifying SEMObject instances
	virtual void handle_event(SDL_Event sdl_event, SEM_EVENT event, SEM_EVENT_EXT ext);

	// Return value of listen/drop events are equivalent to the number of successful listens/drops
	// Start listening to the given event(s)
	virtual int listen_event(SEM_EVENT event);
	virtual int listen_event(SEM_EVENT event, SEM_EVENT_EXT ext);
	virtual int listen_event(vector<pair<SEM_EVENT, SEM_EVENT_EXT>> event_list);

	// Stop listening to the given event(s)
	virtual int drop_event(SEM_EVENT event);
	virtual int drop_event(SEM_EVENT event, SEM_EVENT_EXT ext);
	virtual int drop_event(vector<pair<SEM_EVENT, SEM_EVENT_EXT>> event_list);

	// Stops listening to all events
	virtual int drop_all_events();

private:

};

//////////////////////////////////////////////////////////////////////////////////
//																				//
//	Determine SDL event equivalencies and overload comparator for "SDL_Event".	//
//	Event Cases:																//
//	- Key:			Events are equivalent, if type AND key are equivalent		//
//	- Mouse Click:	Events are equivalent, if type AND button are equivalent	//
//	- Other:		Events are equivalent, if types are equivalent				//
//																				//
//////////////////////////////////////////////////////////////////////////////////

inline bool check_event_equivalence(const SDL_Event& ev1, const SDL_Event& ev2){
	// Both events must have equivalent types
	if(ev1.type != ev2.type) return false;

	// From now on, type of ev1 and ev2 are equal

	// Quit Event
	if(ev1.type == SDL_QUIT) return true;

	// Keyboard Event
	if(ev1.type == SDL_KEYDOWN || ev1.type == SDL_KEYUP ||
		ev1.type == SDL_TEXTEDITING || ev1.type == SDL_TEXTINPUT){
		// For keyboard events, both being the same key is sufficient for equivalence
		if(ev1.key.keysym.sym != ev2.key.keysym.sym) return false;
		return true;
	}

	// Mouse Event
	if(ev1.type == SDL_MOUSEBUTTONDOWN || ev1.type == SDL_MOUSEBUTTONUP){
		if(ev1.button.button != ev2.button.button) return false;
		return true;
	}

	// For other event types, assume type-equivalence as general equivalence and return true
	return true;
}
inline bool operator==(const SDL_Event& ev1, const SDL_Event& ev2){
	return check_event_equivalence(ev1, ev2);
}
inline bool operator!=(const SDL_Event& ev1, const SDL_Event& ev2){
	return !check_event_equivalence(ev1, ev2);
}

#endif // !__INPUT_H__