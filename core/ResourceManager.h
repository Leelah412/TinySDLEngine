#ifndef __RESOURCE_MANAGER__
#define __RESOURCE_MANAGER__

#include "renderer/Texture.h"
#include <map>

#define ResManager ResourceManager::get_default_manager()

// Use the resource manager to ensure, that resources are unique, and that all used resources are freed anytime "delete_all" is called

enum class RES_TYPE {TEXTURE};

class ResourceManager{

public:
	ResourceManager();
	~ResourceManager();

	// Load and return a resource from the given path with the given type
	// Also use this function, if already loaded resource needed
	void* load_resource(const std::string& path, RES_TYPE type, bool force_reload = false){
		if(!force_reload && (m_resources.find(path) != m_resources.end())) return m_resources[path];
		void* res = nullptr;
		// Texture
		if(type == RES_TYPE::TEXTURE){
			res = new Texture(path);
			m_resources.insert(std::pair<std::string, void*>(path, res));
			return res;
		}
		// Load nothing
		return res;
	}

	// Delete the resource corresponding to the given filepath
	// WARNING: make sure to remove all references to the given resource beforehand!!!
	void delete_resource(const std::string& path){
		if(m_resources.find(path) == m_resources.end()) return;
		delete m_resources[path];
	}
	// Remove every resource there is
	// WARNING: same as "delete_resource"
	void delete_all(){
		for(auto& res : m_resources){
			delete res.second;
		}
	}

	// Check, if resource in given path is loaded
	// If desired, also check, if resource saved for path is given resource (to check uniqueness of passed resource)
	bool resource_loaded(const std::string& path, const void* is_res = nullptr){
		if(	m_resources.find(path) != m_resources.end() &&
			( !is_res || (m_resources[path] == is_res) ) )
			return true;

		return false;
	}
	// Check, if resource

	// Note: "ResourceManager" is not an actual singleton per definition, as there can be more than one manager,
	// it's just that usually, we won't need more than one manager at once and having a static variable of class type makes work much easier!

	// Create a new default RM instance, if current singleton is NULL
	static ResourceManager* new_singleton();
	// Delete default RM
	static void delete_singleton();
	// Return the default RM
	static ResourceManager* get_default_manager(){ return m_default; }

private:
	std::map<std::string, void*> m_resources; 	// only one resource per filepath
	static ResourceManager* m_default;			// singleton instance for default use cases

};

#endif // !__RESOURCE_MANAGER__
