#ifndef __RESOURCE_MANAGER__
#define __RESOURCE_MANAGER__

#include "renderer/Resource.h"
#include "renderer/Texture.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"

#include <map>

#define ResManager ResourceManager::get_default_manager()

// Use the resource manager to ensure, that resources are unique, and that all used resources are freed anytime "delete_all" is called

enum class RES_TYPE {TEXTURE, MESH, SHADER};

class Resource;
class ResourceManager{

public:
	ResourceManager();
	~ResourceManager();

	// Load and return a resource from the given path with the given type
	// Use this, if needing a statically used resource (e.g. for textures)
	// WARNING: do NOT mutate the resource in ANY way, UNLESS you want the resource to be mutated for EVERYONE,
	// but most importantly, DO NOT CALL THE DECONSTRUCTOR OF THAT RESOURCE!!!
	// TODO: gotta find a way to disable ability to delete resources by foreign instances
	const Resource* load_resource(const std::string& path, RES_TYPE type, bool force_reload = false){
		using namespace tse;

		if(path == "" || path.empty()) return nullptr;

		if(!force_reload && (m_resources.find(path) != m_resources.end())){
			m_resources[path]->inc();
			return m_resources[path];
		}
		Resource* res = nullptr;
		
		switch(type){
			case RES_TYPE::TEXTURE: {
				res = new Texture(path);
				m_resources.insert(std::pair<std::string, Resource*>(path, res));
				return res;

			}
			case RES_TYPE::MESH: {
				res = new Mesh(path);
				m_resources.insert(std::pair<std::string, Resource*>(path, res));
				return res;
			}
			case RES_TYPE::SHADER: {
				res = new Shader(path);
				m_resources.insert(std::pair<std::string, Resource*>(path, res));
				return res;
			}
			// Load nothing
			default: return nullptr;
		}

		return nullptr;
	}

	// Load a resource, if it doesn't exist, and return a copy
	// Use this, if planning to mutate the resource
	// With this, we don't have to load the same resource from hard disk again and again
	Resource* load_and_copy_resource(const std::string& path, RES_TYPE type, bool force_reload = false){
		using namespace tse;

		if(!force_reload && (m_resources.find(path) != m_resources.end())){
			switch(type){
				case RES_TYPE::TEXTURE: return new Texture(*(Texture*)m_resources[path]);
				case RES_TYPE::MESH: return new Mesh(*(Mesh*)m_resources[path]);
				default: return nullptr;
			}
		}
		Resource* res = nullptr;

		switch(type){
			case RES_TYPE::TEXTURE:{
				res = new Texture(path);
				m_resources.insert(std::pair<std::string, Resource*>(path, res));
				return new Texture(*(Texture*)res);
			
			}
			case RES_TYPE::MESH:{
				res = new Mesh(path);
				m_resources.insert(std::pair<std::string, Resource*>(path, res));
				return new Mesh(*(Mesh*)res);

			}
			default: return nullptr;
		}

		// Load nothing
		return nullptr;
	}

	// "Unload" resource by lowering count
	// Deletes resource, even if not held by ResourceManager
	// Every Resource should be unloaded per ResourceManager, whether it is unique or not
	void unload_resource(Resource* res){
		if(!res) return;

		auto it = m_resources.find(res->get_filepath());
		// unloading unique resource not in list
		if(it == m_resources.end()){
			delete res;
		}
		// unloading resource saved in list
		else if((*it).second == res){
			if(m_resources[(*it).first]->dec() <= 0) delete_resource((*it).first);
		}
		// unloading unique resource
		else{
			delete res;
		}
	}

	// Delete the resource corresponding to the given filepath
	// WARNING: make sure to remove all references to the given resource beforehand!!!
	void delete_resource(const std::string& path){
		if(m_resources.find(path) == m_resources.end()) return;
		// For textures, remove 
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
	bool resource_loaded(const std::string& path, const Resource* is_res = nullptr){
		if(path == "" || path.empty()) return false; 

		if(	m_resources.find(path) != m_resources.end() &&
			( !is_res || (m_resources[path] == is_res) ) )
			return true;

		return false;
	}

	// Note: "ResourceManager" is not an actual singleton per definition, as there can be more than one manager,
	// it's just that usually, we won't need more than one manager at once and having a static variable of class type makes work much easier!

	// Create a new default RM instance, if current singleton is NULL
	static ResourceManager* new_singleton();
	// Delete default RM
	static void delete_singleton();
	// Return the default RM
	static ResourceManager* get_default_manager(){ return m_default; }

private:
	std::map<std::string, Resource*> m_resources; 	// only one resource per filepath
	static ResourceManager* m_default;			// singleton instance for default use cases

};

#endif // !__RESOURCE_MANAGER__
