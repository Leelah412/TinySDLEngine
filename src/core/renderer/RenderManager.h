#ifndef __RENDER_MANAGER_H__
#define __RENDER_MANAGER_H__

#include "VertexArray.h"
#include "Buffer.h"
#include "Framebuffer.h"
#include "Camera.h"
#include "Mesh.h"
#include "UniformBuffer.h"
#include "Light.h"
#include "defaults.h"

#include <vector>
#include <map>

#define USE_RENDER_MANAGER extern tse::RenderManager* current_render_manager;
#define IRenderManager current_render_manager

namespace tse{

typedef std::string ShaderName;
typedef std::string BufferEntityName;
typedef std::string VertexArrayName;
typedef std::string FramebufferName;


// Base interface for all render operations.
// Associated with "TSE_UNIQUE" enum.
// If desired to do something other inheriting structs aren't able to do, override this struct and create your own rendering operation.
typedef struct RenderOperation{
	//TSE_RENDER_ENUM type;

	// One-time setup before rendering
	virtual void setup(){

	}
	// Called each frame
	virtual void render(){

	}
} RenderOperation;


typedef struct RenderState{
	VertexArray* current_vertex_array = nullptr;
	Framebuffer* current_framebuffer = nullptr;
	Shader* current_shader = nullptr;
};

class RenderManager{

public:
	RenderManager();
	virtual ~RenderManager();

	// Add/Remove a render operation to/from the render sequence
	virtual void add_render_operation(RenderOperation* op, int index = -1);
	virtual void remove_render_operation(RenderOperation* op, bool delete_op = true);
	virtual void remove_render_operation(unsigned int index, bool delete_op = true);
	// Remove all render operations
	// Deconstructs all elements, if not specified otherwise
	virtual void clear_render_sequence(bool delete_ops = true);
	const std::vector<RenderOperation*>& get_render_sequence();

	// Get current OpenGL binding state
	const RenderState& get_state();


	/****************/
	/* VERTEX ARRAY */
	/****************/

	virtual void add_vertex_array(VertexArray* vao, const std::string& name);
	virtual void remove_vertex_array(const std::string& name, bool delete_vao = true);
	VertexArray* get_vertex_array(const std::string& name);

	VertexArray* get_vertex_array_state() const;
	virtual void set_vertex_array_state(VertexArray* vao);
	virtual void bind(VertexArray* vao);
	virtual void unbind(VertexArray* vao);


	/***************/
	/* FRAMEBUFFER */
	/***************/

	Framebuffer* get_framebuffer(const std::string& name);

	Framebuffer* get_framebuffer_state() const;
	virtual void set_framebuffer_state(Framebuffer* fbo);
	virtual void bind(Framebuffer* fbo);
	virtual void unbind(Framebuffer* fbo);


	/**********/
	/* SHADER */
	/**********/

	// If shader with given name already exists, it will be overwritten, unless it's the default shader
	virtual void add_shader(Shader* shader, const std::string& name);
	// Removes shader with the given name, if it exists (except default shader)
	virtual void remove_shader(const std::string& name);
	Shader* get_shader(const std::string& name);
	virtual void create_default_shader(const std::string& path = TSE_DEFAULT_SHADER_PATH, bool path_is_shader = false);
	virtual void set_default_shader(Shader* shader);
	Shader* get_default_shader();

	Shader* get_shader_state() const;
	virtual void set_shader_state(Shader* shader);
	virtual void bind(Shader* shader);
	virtual void unbind(Shader* shader);


	/**********/
	/* CAMERA */
	/**********/

	const std::set<Camera*>& get_active_cameras();
	void activate_camera(Camera* camera);
	void deactivate_camera(Camera* camera);


	/*********/
	/* LIGHT */
	/*********/

	const std::set<Light*>& get_active_lights();
	void activate_light(Light* light);
	void deactivate_light(Light* light);

private:
	std::vector<RenderOperation*> m_render_sequence;				// Rendering sequence

	std::set<Camera*> m_active_cameras;								// List of cameras, for which the scene should be rendered TODO: turn it to Camera - Framebuffer map
	std::set<Light*> m_lights;										// Lights to be rendered in the world

	std::map<VertexArrayName, VertexArray*> m_vertex_arrays;		// Each VertexArray is identified by a unique name
	// TODO: create framebuffer repping default framebuffer
	std::map<FramebufferName, Framebuffer*> m_framebuffers;			// Each Framebuffer is identified by a unique name (except default buffer)
	std::map<ShaderName, Shader*> m_shaders;						// Each Shader is identified by a unique name

	RenderState m_state;											// Keep track of the current binding states
};

}


#endif // !__RENDER_MANAGER_H__
