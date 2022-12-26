#ifndef __MESH_H__
#define __MESH_H__

#include "ResourceManager.h"
#include "Texture.h"
#include "Shader.h"
#include "Buffer.h"
#include <glm/glm.hpp>

// Meshes hold vertex data for an object, which can be bound to the vertex buffer for render
// They can also hold specific shaders; if no shader specified, default shaders are used
// Each vertex has the following structure:
// - 3 floats x,y,z for 3D position (if 2D, just set z value to 0)
// - 2 floats u,v for UV coordinates for texture map
// - 2 floats u,v for UV coordinates for normal map
// - 1 unsigned int for transform index in transform buffer
// - 1 unsigned int for material ID

struct MeshVertex{
	glm::vec3 position;
	glm::vec2 uv_tex;
	glm::vec3 uv_normal;
	GLuint transform_index;
	GLuint material_id;
};

struct MeshObject{
	std::vector<MeshVertex> m_vertices;
	std::vector<uint32_t> m_indices;
};

class Mesh{

public:
	Mesh();
	Mesh(MeshObject mesh);
	virtual ~Mesh();

	virtual void bind() const;			// Bind mesh to vertex buffer
	virtual void unbind() const;		// Remove mesh from vertex buffer

	// Load the mesh from given path
	virtual void load_mesh(const std::string& path);
	// Load texture; if desired, with resource manager
	virtual void load_texture(const std::string& path, bool unique = false);
	virtual void load_normal(const std::string& path, bool unique = false);
	// Load the same resources again, if resource manager contains it, but this time don't bind it to RM
	virtual void make_texture_unique();
	virtual void make_normal_unique();

private:
	MeshObject m_mesh_object;
	Texture* m_texture = nullptr;		// Reference to texture containing texture map
	Texture* m_normal = nullptr;		// Reference to texture containing normal map

};

#endif // !__MESH_H__
