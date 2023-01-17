#ifndef __MODEL_NODE_H__
#define __MODEL_NODE_H__

#include "Node.h"
#include "renderer/ModelManager.h"
#include <renderer/Mesh.h>
#include <renderer/Material.h>
#include <renderer/Shader.h>
#include <renderer/Texture.h>
#include <ResourceManager.h>

#include <string>
#include <vector>

// TODO: turn Mesh into ModelNode, and turn ModelNode to ModelNode

// Abstract base class for all models (expected) to be displayed in the game world
// Consists of a mesh and material

namespace tse{

class ModelNode : public Node{

public:
	ModelNode();
	ModelNode(const std::string& path, bool unique = false);
	ModelNode(Model* model);
	virtual ~ModelNode();

	virtual void update(const time_t& delta) override;

	// Load model from given path
	virtual bool load_model(const std::string& path, bool unique = false);
	virtual bool set_model(Model* model);
	virtual void delete_model();

	Model* get_model() const;
	// Create a unique copy specifically for this model, if we are using a mesh from the resource manager
	// TODO: put this to "Model"
	//virtual void make_mesh_unique();

	bool is_visible() const;
	virtual void set_visibility(bool visible);

#if 0
	// TEXTURE
	// Load texture; if desired, with resource manager
	virtual void load_texture(const std::string& path, bool unique = false);
	// NORMAL
	// Load texture; if desired, with resource manager
	virtual void load_normal(const std::string& path, bool unique = false);
	// Load the same resources again, if resource manager contains it, but this time don't bind it to RM
	virtual void make_texture_unique();
	virtual void make_normal_unique();
#endif

protected:
	Model* m_model = nullptr;

	bool m_visible = true;				// Don't render, if set invisible
};

}


#endif // !__MODEL_NODE_H__
