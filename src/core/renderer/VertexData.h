#ifndef __RENDER_OBJECT_H__
#define __RENDER_OBJECT_H__

#include "Resource.h"

#include <GL/glew.h>
#include <vector>
#include <set>
#include <memory>
#include <iostream>
#include <string>

// Container for Vertex- and Index data
// Does not care about layout, only holds raw bytes
class VertexData : public Resource{

public:
	VertexData();
	VertexData(const void* vertex_data, GLuint vertex_size, const std::vector<GLuint>& indices);
	virtual ~VertexData();

	// Set new vertices
	// Deletes previous vertices!
	bool set_vertices(const void* data, GLuint size);
	// Update vertices with new data starting at given offset
	bool update_vertices(const void* data, GLuint offset, GLuint size);
	// Add vertices inbetween
	bool add_vertices(const void* data, GLuint offset, GLuint size);
	// Delete vertices in given range
	bool delete_vertices(GLuint offset, GLuint size);
	const void* get_vertices() const;
	GLuint get_vertex_array_size() const;

	// Set new indices
	// Deletes previous indices!
	bool set_indices(const std::vector<GLuint>& indices);
	// Update indices with new data starting at given offset
	bool update_indices(const std::vector<GLuint>& indices, GLuint offset);
	// Add vertices inbetween
	bool add_indices(const std::vector<GLuint>& indices, GLuint offset);
	// Delete vertices in given range
	bool delete_indices(GLuint offset, GLuint size);
	const std::vector<GLuint>& get_indices() const;

private:
	void* m_vertices;													// Vertices with attribute values
	GLuint m_vertex_array_size;											// Size of the vertex array in bytes
	
	// TODO: turn this into a raw GLuint pointer without using std::vector
	std::vector<GLuint> m_indices;										// Indices forming the tris of the geometry object
	//GLuint m_index_count;
};

#endif // !__RENDER_OBJECT_H__
