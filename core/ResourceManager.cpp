#include "ResourceManager.h"

ResourceManager* ResourceManager::m_default = new ResourceManager();

ResourceManager::ResourceManager() : m_resources{}{}
ResourceManager::~ResourceManager(){
	delete_all();
}

ResourceManager* ResourceManager::new_singleton(){
	if(m_default) return m_default;
	return m_default = new ResourceManager();
}

void ResourceManager::delete_singleton(){
	delete m_default;
	m_default = nullptr;
}
