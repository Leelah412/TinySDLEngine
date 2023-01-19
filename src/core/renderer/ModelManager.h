#ifndef __MODEL_MANAGER_H__
#define __MODEL_MANAGER_H__

#include "RenderManager.h"
#include "Model.h"
#include "VertexArray.h"
#include "UniformBuffer.h"
#include "Light.h"

#include <vector>
#include <map>
#include <algorithm>

#define IModelManager tse::ModelManager::get_default_model_manager()

USE_RENDER_MANAGER

namespace tse{


//struct MeshRender;

// Manages the models in the world.
// Should be used to add or remove Models, or to update a Mesh, VertexData, Naterial or Shader of a Model instance.
// Doing these without ModelManager could and will lead to faulty results.
class ModelManager{

public:
	ModelManager();
	virtual ~ModelManager();

	// Initialize the vertex and index buffer
	virtual void init_world();

	// Adds model to render pipeline
	virtual void add_model(Model* model);
	virtual void update_model(Model* model);
	// Remove model from render pipeline
	virtual void remove_model(Model* model);

	// Call this to update the material of a models submesh.
	// Model is passed for verification purposes
	//virtual void update_submesh_material(Material* material, VertexData* submesh, Model* model);
	// Replace mesh of model with given mesh
	//virtual void update_model_mesh(Mesh* mesh, Model* model);

	const std::set<Model*>& get_models() const;
	const std::set<VertexMaterial*, VertexMaterial::less>& get_world() const;
	VertexArray* get_vertex_array() const;
	const DisjointInterval& get_vertex_buffer_interval(VertexMaterial* vm) const;
	const DisjointInterval& get_index_buffer_interval(VertexMaterial* vm) const;

	const UniformBuffer* get_light_ubo() const{ return m_light_ubo; };
	// Resizes the light uniform buffer and passes the given data 
	// TODO: instead of resizing every frame, set a maximum to the nr. of possible lights, make ubo persistent,
	// pass an integer indicating the number of lights at the front of the uniform, and subdata all lights behind it
	void resize_light_ubo(GLuint size, const void* data, GLuint offset, GLuint data_size){
		m_light_ubo->alloc_buffer(size);
		m_light_ubo->push(data, offset, data_size);
	}
	void bind_base_light_ubo(GLuint index){
		glBindBufferBase(GL_UNIFORM_BUFFER, index, m_light_ubo->get_buffer_id());
	}

	static ModelManager* get_default_model_manager();

private:

	std::set<Model*> m_models = {};												// Models to be rendered
	std::unordered_map<Model*, std::set<VertexMaterial*>> m_cur_submeshes = {};	// Currently saved submeshes of the model
	std::set<VertexMaterial*, VertexMaterial::less> m_world = {};				// Contains all vertex and material information to be rendered in the game world

	VertexArray* m_vao = nullptr;												// VAO containing the models
	
	// The following lists follow the spaces in the buffers, which VertexData is assigned at which interval, and where we have empty spaces.
	// Since we follow the intervals for VertexMaterials, and not for Meshes or Models, we can safely fragment a Mesh to use the entire space of a buffer.
	// Removing a Model in its entirety is done with the help of the VertexMaterial pointers saved in "m_cur_submeshes"

	std::map<DisjointInterval, VertexMaterial*> m_interval_vertex = {};			// Mapping VBO vertices to intervals, sorted by intervals
	std::map<VertexMaterial*, DisjointInterval> m_vertex_interval = {};			// Intervals for the vertices in the VBO
	std::set<DisjointInterval> m_free_vbo_intervals = {};						// Free space intervals inside VBO
	std::multimap<GLuint, DisjointInterval> m_vbo_space = {};					// Keep track of the free intervals inside the VBO, sorted by free vertex count (not size in bytes!)
	std::map<DisjointInterval, VertexMaterial*> m_interval_index = {};			// Mapping IBO vertices to intervals, sorted by intervals
	std::map<VertexMaterial*, DisjointInterval> m_index_interval = {};			// Intervals for the indices in the IBO
	std::set<DisjointInterval> m_free_ibo_intervals = {};						// Free space intervals inside IBO
	std::multimap<GLuint, DisjointInterval> m_ibo_space = {};					// Keep track of the free intervals inside the IBO, sorted by free index count

	GLuint m_expected_max_vertices = 0xFFFFFF;									// Max. vertices expected to be in the world throughout a scene | Default: 2^24 (~16.7mil)
	GLuint m_expected_max_indices = 0xFFFFFF;									// Max. indices expected to be in the world throughout a scene

	UniformBuffer* m_light_ubo = nullptr;										// UBO holding light structs

	static ModelManager* s_default;												// Default ModelManager 

	// Find the smallest interval big enough to fit the given amount of vertices (NOT bytes!)
	// Returns the iterator to an ideal interval, or end(), if no space available
	std::multimap<GLuint, DisjointInterval>::iterator find_ideal_vertex_interval(GLuint count);

	// Find the smallest interval big enough to fit the given amount of indices
	// Returns the iterator to an ideal interval, or end(), if no space available
	std::multimap<GLuint, DisjointInterval>::iterator find_ideal_index_interval(GLuint count);

	// Reallocate VBO
	void realloc_vertex_buffer(GLuint size, bool is_static = false);
	// Reallocate IBO
	void realloc_index_buffer(GLuint count, bool is_static = false);
	// Left-shift VBO memory
	void left_shift_vertex_buffer();
	// Left-shift IBO memory
	void left_shift_index_buffer();

	// Return number of vertices, which fit into the current VBO (not a byte size!)
	GLuint get_free_vbo_space();
	// Return number of indices, which fit into the current IBO	
	GLuint get_free_ibo_space();

	// Add interval indicating row of free spaces in the vertex buffer
	void add_free_vbo_space(const DisjointInterval& di);
	// Add interval indicating row of free spaces in the index buffer
	void add_free_ibo_space(const DisjointInterval& di);

	// Remove the given interval from the list of free spaces in the vertex buffer
	void remove_free_vbo_space(const DisjointInterval& di);
	// Remove the given interval from the list of free spaces in the index buffer
	void remove_free_ibo_space(const DisjointInterval& di);

	// Return all VertexMaterials in the given vertex buffer range (inclusive)
	std::set<VertexMaterial*> get_vertex_materials_in_vbo_range(const DisjointInterval& di);
	// Return all VertexMaterials in the given index buffer range (inclusive)
	std::set<VertexMaterial*> get_vertex_materials_in_ibo_range(const DisjointInterval& di);

	// Move the vertex materials in the VBO to the given interval
	// Only works, if desired VBO space not occupied
	// Assumes, that free space intervals aren't neighbors
	void move_vertex_materials_in_vbo(VertexMaterial* vm, const DisjointInterval& new_iv);
	
	// Move the vertex materials in the IBO to the given interval
	// Only works, if desired IBO space not occupied
	void move_vertex_materials_in_ibo(VertexMaterial* vm, const DisjointInterval& new_iv);

	// Concatenates neighboring free space intervals for the vertex buffer
	void concat_free_vbo_space();

	// Concatenates neighboring free space intervals for the index buffer
	void concat_free_ibo_space();

	// Calculate expected vertex and index count from existing Models
	void calc_expected_max_vertices_indices();

};


// Render Mesh instances together with their assigned materials
typedef struct MeshRender : public RenderOperation{

	virtual void setup() override{
		// TODO: allow unique setup
		// 
		// Enable 2D Textures
		glEnable(GL_TEXTURE_2D);
		// Enable transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// Enable depth buffer
		glEnable(GL_DEPTH_TEST);
		// Enable back face culling
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
	}
	virtual void render() override{
		if(!IModelManager->get_vertex_array()) return;

		// get cameras
		const std::set<Camera*>& cameras = IRenderManager->get_active_cameras();
		// no camera -> nothing to capture scene with
		if(!cameras.size()) return;

		// get light sources
		const std::set<Light*>& lights = IRenderManager->get_active_lights();
		// set light ubo
		int sizeoflight = lights.size() * sizeof(Light);
		float* data = new float[sizeoflight / 4];
		int i = 0;
		std::vector<Light> l_vec;
		for(auto l : lights){
			//l_vec.push_back(*l);
			std::memcpy(&data[i * sizeof(Light) / 4 + 0], &l->direction, 16);
			std::memcpy(&data[i * sizeof(Light) / 4 + 4], &l->ambient, 12);
			std::memcpy(&data[i * sizeof(Light) / 4 + 7], &l->diffuse, 12);
			std::memcpy(&data[i * sizeof(Light) / 4 + 10], &l->specular, 12);
			std::memcpy(&data[i * sizeof(Light) / 4 + 13], &l->constant, 4);
			std::memcpy(&data[i * sizeof(Light) / 4 + 14], &l->linear, 4);
			std::memcpy(&data[i * sizeof(Light) / 4 + 15], &l->quadratic, 4);
			i++;
		}
		//IModelManager->resize_light_ubo(sizeoflight, (const void*)l_vec.data(), 0, sizeoflight);
		IModelManager->resize_light_ubo(sizeoflight, (const void*)data, 0, sizeoflight);
		IModelManager->bind_base_light_ubo(0);
		delete[] data;

		// bind vao
		IRenderManager->bind(IModelManager->get_vertex_array());
		const std::set<VertexMaterial*, VertexMaterial::less>& world = IModelManager->get_world();
		// shader state might have changed, without render manager knowing
		// reset shader state
		IRenderManager->bind((Shader*)nullptr);
		Shader* prev_sh = nullptr;

		// TODO: for camera{ for shader{ for material { <render models with same material + no tmp. uniform changes as batch>
		// render world once for each active camera
		for(auto cam : cameras){
			// set viewport to camera
			glViewport(cam->get_viewport_x(), cam->get_viewport_y(), cam->get_viewport_width(), cam->get_viewport_height());

			for(auto vm : world){
				if(!vm->vertex_data) continue;

				const DisjointInterval& v = IModelManager->get_vertex_buffer_interval(vm),
										i = IModelManager->get_index_buffer_interval(vm);

				// no vertices somehow for current VM inside world
				if(v.start == -1) continue;

				// no material or shader -> render default shader
				if(!vm->material || !vm->material->get_shader()){
					// bind default shader, if no material or shader applied
					IRenderManager->bind(IRenderManager->get_default_shader());
				}
				// material exists with shader
				else{
					// bind material
					IRenderManager->bind(vm->material->get_shader_mutable());
					vm->material->bind();
				}

				// if bound new shader, set camera uniform and bind lights
				if(prev_sh != IRenderManager->get_shader_state()){
					prev_sh = IRenderManager->get_shader_state();
					// "activate" camera by setting mvp matrix uniform in current shader
					prev_sh->set_uniform_mat4f(TSE_DEFAULT_SHADER_MVP_UNIFORM, cam->get_camera_view());
					// bind lights
					glUniformBlockBinding(prev_sh->get_program_id(), IModelManager->get_light_ubo()->get_buffer_id(), 0);
				}

				// render submesh				
				// index buffer interval exists
				if(i.start != -1){
					glDrawElementsBaseVertex(
						GL_TRIANGLES,
						vm->vertex_data->get_indices().size(),
						GL_UNSIGNED_INT,
						(void*)(i.start * sizeof(GLuint)),
						v.start);
				}
				// index buffer interval does not exist
				else{
					glDrawArrays(
						GL_TRIANGLES,
						v.start,
						v.length());
				}
			}
		}

	}
};



}

#endif // !__MODEL_MANAGER_H__

