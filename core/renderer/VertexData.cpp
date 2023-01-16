#include "VertexData.h"

VertexData::VertexData(){
	m_vertices = nullptr;
	m_vertex_array_size = 0;
	m_indices = {};
}
VertexData::VertexData(const void* vertex_data, GLuint vertex_size, const std::vector<GLuint>& indices){
	set_vertices(vertex_data, vertex_size);
	set_indices(indices);
}
VertexData::~VertexData(){
	// TODO: might cause problems with resource manager later on, so unload resource, before deleting render object!!!
	delete[] m_vertices;
}

bool VertexData::set_vertices(const void* data, GLuint size){
	delete[] m_vertices;
	m_vertices = nullptr;
	m_vertex_array_size = 0;
	return add_vertices(data, 0, size);
}

bool VertexData::update_vertices(const void* data, GLuint offset, GLuint size){
	if(!size) return false;
	if(offset + size > m_vertex_array_size){
		std::cout << "WARNING: Trying to update inaccessible memory. Abort." << std::endl;
		return false;
	}
	if(!data){
		std::cout << "WARNING: Passed NULL as data. Abort." << std::endl;
		return false;
	}

	// copy new data starting from offset
	std::memcpy((unsigned char*)m_vertices + offset, data, size);
}

bool VertexData::add_vertices(const void* data, GLuint offset, GLuint size){
	if(!size) return false;
	if(offset > m_vertex_array_size){
		std::cout << "WARNING: Trying to add at higher offset than possible. Abort." << std::endl;
		return false;
	}
	if(!data){
		std::cout << "WARNING: Passed NULL as data. Abort." << std::endl;
		return false;
	}

	// extend memory
	void* new_array = new unsigned char[m_vertex_array_size + size];
	if(!new_array){
		std::cerr << "ERROR: Couldn't allocate more memory. Abort update." << std::endl;
		return false;;
	}

	// copy old data until offset
	std::memcpy(new_array, m_vertices, offset);
	// copy new data starting from offset
	std::memcpy((unsigned char*)new_array + offset, data, size);
	// copy old data starting from offset + size
	std::memcpy((unsigned char*)new_array + offset + size, m_vertices, m_vertex_array_size - offset);

	// remove previous allocation and assign new array
	delete[] m_vertices;
	m_vertices = new_array;
	m_vertex_array_size += size;
	return true;
}

bool VertexData::delete_vertices(GLuint offset, GLuint size){
	if(!size) return false;
	if(offset > m_vertex_array_size){
		std::cout << "WARNING: Trying to delete at higher offset than possible. Abort." << std::endl;
		return false;
	}

	// lower memory
	void* new_array = new unsigned char[m_vertex_array_size - size];
	if(!new_array){
		std::cerr << "ERROR: Couldn't reallocate memory. Abort update." << std::endl;
		return false;;
	}

	// copy old data
	std::memcpy(new_array, m_vertices, offset);
	std::memcpy((unsigned char*)new_array + offset + size, m_vertices, m_vertex_array_size - offset);

	// remove previous allocation and assign new array
	delete[] m_vertices;
	m_vertices = new_array;
	m_vertex_array_size -= size;
	return true;

}
const void* VertexData::get_vertices() const{
	return m_vertices;
}
GLuint VertexData::get_vertex_array_size() const{
	return m_vertex_array_size;
}

bool VertexData::set_indices(const std::vector<GLuint>& indices){
	m_indices = {};
	m_indices.insert(m_indices.end(), indices.begin(), indices.end());
	return true;
}
bool VertexData::update_indices(const std::vector<GLuint>& indices, GLuint offset){
	if(indices.empty()) return true;
	if(offset + indices.size() > m_indices.size()){
		std::cout << "WARNING: Trying to update inaccessible memory. Abort." << std::endl;
		return false;
	}

	for(GLuint i = 0; i < indices.size(); i++){
		m_indices.at((size_t)i + offset) = indices.at(i);
	}

	return true;
}
bool VertexData::add_indices(const std::vector<GLuint>& indices, GLuint offset){
	if(indices.empty()) return true;
	if(offset >= m_indices.size()){
		std::cout << "WARNING: Trying to add at inaccessible memory. Abort." << std::endl;
		return false;
	}

	std::vector<GLuint> new_indices;
	new_indices.insert(new_indices.end(), m_indices.begin(), m_indices.begin() + offset);
	new_indices.insert(new_indices.end(), indices.begin(), indices.end());
	new_indices.insert(new_indices.end(), m_indices.begin() + offset, m_indices.end());
	m_indices = new_indices;

	return true;
}
bool VertexData::delete_indices(GLuint offset, GLuint size){
	if((offset + size) >= m_indices.size()){
		std::cout << "WARNING: Trying to delete non-existing data. Abort." << std::endl;
		return false;
	}

	std::vector<GLuint> new_indices;
	new_indices.insert(new_indices.end(), m_indices.begin(), m_indices.begin() + offset);
	new_indices.insert(new_indices.end(), m_indices.begin() + offset + size, m_indices.end());
	m_indices = new_indices;

	return true;

}

const std::vector<GLuint>& VertexData::get_indices() const{
	return m_indices;
}