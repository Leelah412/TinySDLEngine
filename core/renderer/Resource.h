#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <string>

// Base class for all resources, that can be handled by resource manager
// Contains path to resource and a count of instances holding the resource
class Resource{

public:
	Resource(){ m_count = 1; }
	virtual ~Resource(){}

	const std::string& get_filepath() const{ return m_filepath; }

	unsigned int get_count() const{ return m_count; }

	// Increment/Decrement count and return new count
	unsigned int inc(){ return ++m_count; }
	unsigned int dec(){ return --m_count; }

protected:
	std::string m_filepath = "";

private:
	unsigned int m_count = 0;

};

#endif // !__RESOURCE_H__
