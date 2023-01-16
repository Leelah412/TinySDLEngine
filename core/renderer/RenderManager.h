#ifndef __RENDER_MANAGER_H__
#define __RENDER_MANAGER_H__

#include "VertexArray.h"
#include "Buffer.h"
//#include "BufferEntity.h"
#include "Framebuffer.h"
#include "Camera.h"
#include "Mesh.h"
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

typedef enum TSE_RENDER_ENUM{
	TSE_UNIQUE = 0,
	TSE_CAMERA_CAPTURE,
	TSE_RENDER_ARRAY,
	TSE_FRAMEBUFFER_TRANSFER,
	// Following are TODOs
	TSE_RENDER_TEXTURE,
	TSE_RENDER_UI
} TSE_RENDER_ENUM;

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
	//BufferEntity* current_buffer_entity = nullptr;
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

#ifdef __BUFFER_OBJECT_H__
	/*****************/
	/* BUFFER ENTITY */
	/*****************/

	virtual void add_buffer_entity(BufferEntity* bo, const std::string& name);
	virtual void remove_buffer_entity(const std::string& name, bool delete_bo = true);
	BufferEntity* get_buffer_entity(const std::string& name);

	// Add/refresh/delete a mesh to/in/from the buffer entity
	virtual void add_buffer_entity_object(std::string bo_name, RenderObject* object);
	virtual void refresh_buffer_entity_object(std::string bo_name, RenderObject* object);
	virtual void remove_buffer_entity_object(std::string bo_name, RenderObject* object);

	BufferEntity* get_buffer_entity_state() const;
	virtual void set_buffer_entity_state(BufferEntity* bo);
	virtual void bind(BufferEntity* bo, bool bind_shader);
	virtual void unbind(BufferEntity* bo, bool unbind_shader);
#endif

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

private:
	std::vector<RenderOperation*> m_render_sequence;				// Rendering sequence

	std::set<Camera*> m_active_cameras;								// List of cameras, for which the scene should be rendered TODO: turn it to Camera - Framebuffer map
	std::map<VertexArrayName, VertexArray*> m_vertex_arrays;		// Each VertexArray is identified by a unique name
	//std::map<BufferEntityName, BufferEntity*> m_buffer_entities;	// Each BufferEntity is identified by a unique name
	// TODO: create framebuffer repping default framebuffer
	std::map<FramebufferName, Framebuffer*> m_framebuffers;			// Each Framebuffer is identified by a unique name (except default buffer)
	std::map<ShaderName, Shader*> m_shaders;						// Each Shader is identified by a unique name

	RenderState m_state;											// Keep track of the current binding states
};

}

USE_RENDER_MANAGER

namespace tse{


#if 0
// Render scene contained in given VAO into target FBO.
// Example usage: Capture depth map for a light source.
// Associated with "TSE_CAMERA_CAPTURE" enum.
typedef struct CameraCapture : public RenderOperation{
	Camera* camera;
	FramebufferName framebuffer_target;
	// VAO to render
	// TODO: make this an array of vaos
	VertexArrayName vertex_array;
#if 0

	// List of buffer entities to render with the given shader (only relevant, if "render_buffer_entities" true)
	// Use default buffer entity shader, if no shader specified
	// Useful, if for example one wants to render only a subset of a VAO, or if one wants to use another shader
	std::vector<std::pair<BufferEntityName, ShaderName>> buffer_entities;
	// True: Render given buffer entities on default VAO, ignore "vertex_array"
	// False: Render given VAO, ignore "buffer_entities" (default)
	bool render_buffer_entities = false;
#endif

	virtual void render(){
#if 0
		// use default vao, to which we bind and then unbind the given buffer entities
		if(render_buffer_entities){
			render_on_default_vao();
			return;
		}
#endif

		VertexArray* vao = IRenderManager->get_vertex_array(vertex_array);
		if(!vao) return;

		IRenderManager->bind(vao);

		for(std::vector<BufferEntity*>::iterator it = vao->begin(); it != vao->end(); it++){
			if(!(*it)){
				std::cout << "WARNING: VertexArray has a NULL BufferEntity. Continue." << std::endl;
				continue;
			}
			render_buffer_entity(*it);
		}
	}

private:

#if 0
	virtual void render_on_default_vao(){
		VertexArray* vao = IRenderManager->get_default_vertex_array();
		if(!vao) return;

		IRenderManager->bind(vao);

		BufferEntity* bo;
		Shader *sh, *tmp;
		for(unsigned int i = 0; i < buffer_entities.size(); i++){
			bo = IRenderManager->get_buffer_entity(buffer_entities.at(i).first);
			// try to get given shader, otherwise assign default shader
			if(!(sh = IRenderManager->get_shader(buffer_entities.at(i).second))) sh = bo->get_shader();

			// can't work without shaders
			if(!sh){
				std::cout << "WARNING: No shader assigned. Can't render BufferEntity. Continue." << std::endl;
				continue;
			}

			// temporarily assign new shader
			tmp = bo->get_shader();
			bo->set_shader(sh);

			render_buffer_entity(bo);
			IRenderManager->unbind(bo, false);		// don't unbind shader!

			// reassign previous shader
			bo->set_shader(tmp);
		}
	}
#endif

	// Activate camera and bind framebuffer target
	virtual void activate_camera_target(){
		// "activate" camera by setting mvp matrix uniform in current shader
		Shader* sh = IRenderManager->get_shader_state();
		sh->set_uniform_mat4f(TSE_DEFAULT_MVP_UNIFORM, camera->get_camera_view());
		// set viewport to size
		glViewport(camera->get_viewport_x(), camera->get_viewport_y(), camera->get_viewport_width(), camera->get_viewport_height());

		// bind framebuffer
		// TODO: for now, we just draw into main window
#if 0
		if(framebuffer_target == "" || framebuffer_target.empty()){
			IRenderManager->bind((Framebuffer*) nullptr);
		}
		else if(!IRenderManager->get_framebuffer(framebuffer_target)) return;
		IRenderManager->bind(IRenderManager->get_framebuffer(framebuffer_target));
#endif

	}

	// TODO: dont bind vbos, bind materials and make vbos already bound to vaos!
	virtual void render_buffer_entity(BufferEntity* bo){
		if(!bo->get_shader()){
			std::cout << "WARNING: No shader assigned. Can't render BufferEntity. Continue." << std::endl;
			return;
		}
		IRenderManager->bind(bo, true);			// bind BO including shader
		activate_camera_target();				// activate camera
		bo->render();							// render the BO into the recently set framebuffer based on camera view
	}

} CameraCapture;

// Draw VBO in BufferObject.
// Example usage: Draw rendered scene texture onto window in a specific form.
// Associated with "TSE_RENDER_ARRAY" enum.
typedef struct DrawArray : public RenderOperation{
	std::string buffer_object;
	virtual void render(){

	}
} DrawArray;


// Render source FBO to target FBO.
// Example usage: Capture scene from different player perspectives and render them onto specific parts of the screen.
// Associated with "TSE_FRAMEBUFFER_TRANSFER" enum.
// nullptr equates to default FBO.
typedef struct FramebufferTransfer : public RenderOperation{
	std::string src_fbo;
	glm::vec4 src_rect;
	std::string dst_fbo;
	glm::vec4 dst_rect;
	virtual void render(){

	}
} FramebufferTransfer;

#endif

}


#endif // !__RENDER_MANAGER_H__
