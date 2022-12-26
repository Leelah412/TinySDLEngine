#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>
#include <SDLEventManager.h>

#define DEBUG 1

using namespace std;
typedef string SIGNAL;

// Event data passed to listeners, when emitting a signal,
// and which can be extended by inheriting classes to add more information, if needed

class Object;

class EVENT{
public:
	EVENT(SIGNAL _signal, Object* _subject) : signal{_signal}, subject{_subject}{};
	virtual ~EVENT(){};

	SIGNAL signal;
	Object* subject;

};

typedef set<Object*> OBSERVER_LIST;
class Object : public SEMObject{

public:

	Object();
	~Object();

	// Override this function to receive 
	virtual void update_on_signal(SIGNAL signal, EVENT* event);				// function to call, when updating an observer

protected:

	virtual void emit_signal(SIGNAL signal, EVENT* event);					// subject emits signal (i.e. call "update_on_signal" function for all subscribed objects)

	void add_listener(SIGNAL signal, Object* observer);						// observer listens to signal of this instance
	void remove_listener(SIGNAL signal, Object* observer);					// observer stops listening to signal of this instance

	map<SIGNAL, OBSERVER_LIST> m_signal_observers;							// list of objects observing a signal for each signal

	/*
	// STATIC OBSERVER
	static void emit_static_signal(SIGNAL signal, Object* subject);				// subject emits static signal

	static void add_static_listener(SIGNAL signal, Object* observer);			// observer listens to static signal
	static void remove_static_listener(SIGNAL signal, Object* observer);

	static map<SIGNAL, OBSERVER_LIST> m_static_signal_observers;				// list of objects observing a signal for each signal
	//static map<const Object*, set<SIGNAL>> m_static_observed_signals;			// list of signals observed by an observer TODO: currently not needed
	*/

private:

};

#endif // !__OBJECT_H__
