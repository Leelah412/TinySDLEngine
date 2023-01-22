#ifndef __MESH_H__
#define __MESH_H__

#include "Resource.h"
#include "VertexData.h"
#include "Material.h"
#include "ObjLoader.h"

#include <string>
#include <vector>
#include <set>

namespace tse{


// Meshes are containers holding VertexData - Material pairings for a single complete mesh object.
// A single VertexData would be seen as a submesh of a complete mesh object.
// For example, a house mesh may contain multiple different submeshes (like doors, windows, walls), all with different materials and maybe even different shaders.
// Each submesh (i.e. VertexData) would then get a material assigned. If no specific material is used, the default material will be assigend
class Mesh : public Resource{

public:
	Mesh();
	// Construct mesh from file
	Mesh(const std::string& path);
	// Construct mesh from raw vertex and index data
	Mesh(const void* vertex_data, GLuint vertex_size, const std::vector<GLuint>& indices);
	virtual ~Mesh();

	// Load mesh from file
	virtual void load_mesh(const std::string& path);
	// Save mesh to file
	virtual void save_mesh(const std::string& path);
	// Add submesh (TODO: at the given index, if desired)
	virtual void add_submesh(VertexData* mesh, GLint idx = -1);
	// Add a submesh from raw data
	virtual void add_submesh(const void* vertex_data, GLuint vertex_size, const std::vector<GLuint>& indices);
	// Return submesh at given index
	VertexData* get_submesh(GLuint idx) const;
	VertexData* get_submesh(VertexData* mesh) const;
	// Check, if submesh is part of mesh
	bool has_submesh(VertexData* mesh) const;
	// Remove submesh at given index
	virtual void remove_submesh(GLuint idx);
	virtual void remove_submesh(VertexData* mesh);
	const std::vector<VertexData*>& get_submesh_list() const;
	// Remove all currently attached submeshes
	virtual bool delete_mesh();

	// Return the size of the mesh in bytes
	unsigned int mesh_vertex_size() const;
	// Return the number of the mesh indices 
	unsigned int mesh_index_count() const;

private:
	std::vector<VertexData*> m_submeshes;			// Submeshes of the mesh
	bool m_modified = true;							// A Mesh is considered to be modified, if the data is/becomes inequivalent to the data in the filepath
};

}


#endif // !__MESH_H__
