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

// Model class containing a Mesh and its Submesh - Material combinations.
// Model is NOT a Resource, but instead each Model instance is unique,
// while the Mesh and the Materials used by its submeshes are loaded with ResourceManager
// and can be non-unique (though in cases, where vertices are modified, that should not be the case)!
class Model{

public:
	Model();
	// Creates a Model with its Meshes and Materials from the given .model file
	// If "unique" is true, the created Meshes will be unique to the Model
	Model(const std::string& path, bool unique = true);
	Model(Mesh* mesh, bool unique = true);
	virtual ~Model();

	// Creates a Model with its Meshes and Materials from the given .model file
	// If "unique" is true, the created Meshes will be unique to the Model
	bool load_model(const std::string& path, bool unique = true);
	// Load Mesh from path and determine, whether the data should be unique to the Model
	bool load_mesh(const std::string& path, bool unique = true);
	// Save mesh to the given path
	// Saving will (force)reload the mesh from the given path again, so we have to tell again,
	// whether we want the mesh to be unique to the current Model or not
	//bool save_mesh(const std::string& path, bool unique = true);
	// Set current mesh to given mesh
	bool set_mesh(Mesh* mesh, bool unique = true);
	// Don't use current mesh
	void reset_mesh();
	Mesh* get_mesh() const;

	const std::unordered_map<VertexData*, VertexMaterial>& get_vertex_material_map() const;
	std::vector<Ref<VertexMaterial>> get_vertex_materials();

	virtual void add_vertex_material(VertexData* v, Material* m, std::vector<Uniform> uniform_changes = {});
	// Assign material to an existing VertexMaterial
	virtual void assign_material(VertexData* v, Material* m, std::vector<Uniform> uniform_changes = {});
	// Remove material of the given vertex data
	virtual void remove_material(VertexData* v);

	unsigned int get_model_id() const;

protected:
	Mesh* m_mesh;															// Reference to the mesh of the model
	std::unordered_map<VertexData*, VertexMaterial> m_vertex_materials;		// Applies Materials to Mesh data

	// Determines, whether we need to create a new file for the model, when saving the scene or can point to an existing one
	// Non-unique assumes, that the Mesh is loaded from its given Resource path
	// For unique Meshes, we have to differentiate between 
	//bool m_unique = false;

	// Initialize material list for the submeshes of the current mesh, without any assigned material
	// TODO: assign default material
	virtual void init_vertex_material_list();

private:
	static unsigned int m_model_id_count;
	unsigned int m_model_id;

};

}


#endif // !__MODEL_H__
