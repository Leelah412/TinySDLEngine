#ifndef __MODEL_H__
#define __MODEL_H__

#include "ResourceManager.h"
#include "Mesh.h"
#include "math/Interval.h"
#include "VertexMaterial.h"
#include "defaults.h"

#include <vector>
#include <map>
#include <functional>


namespace tse{

class Model{

public:
	Model();
	Model(const std::string& path, bool unique = true);
	Model(Mesh* mesh, bool unique = true);
	virtual ~Model();

	// Load mesh from path
	bool load_mesh(const std::string& path, bool unique = true);
	// Set current mesh to given mesh
	bool set_mesh(Mesh* mesh, bool unique = true);
	// Don't use current mesh
	void reset_mesh();
	const Mesh* get_mesh();

	const std::unordered_map<VertexData*, VertexMaterial>& get_vertex_material_map();
	const std::vector<Ref<VertexMaterial>> get_vertex_materials();

	virtual void add_vertex_material(VertexData* v, Material* m, std::vector<Uniform> uniform_changes = {});
	// Assign material to an existing VertexMaterial
	virtual void assign_material(VertexData* v, Material* m, std::vector<Uniform> uniform_changes = {});
	// Remove material of the given vertex data
	virtual void remove_material(VertexData* v);

	unsigned int get_model_id();

protected:
	Mesh* m_mesh;															// Reference to the mesh of the model
	std::unordered_map<VertexData*, VertexMaterial> m_vertex_materials;		// Applies Materials to Mesh data

	// Initialize material list for the submeshes of the current mesh, without any assigned material
	// TODO: assign default material
	virtual void init_vertex_material_list();

private:
	static unsigned int m_model_id_count;
	unsigned int m_model_id;

};

}


#endif // !__MODEL_H__
