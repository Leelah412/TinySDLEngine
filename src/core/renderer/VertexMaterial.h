#ifndef __VERTEX_MATERIAL_H__
#define __VERTEX_MATERIAL_H__

#include "Material.h"
#include "VertexData.h"

namespace tse{

// Represents the general combination of VBO/IBO data and the material applied to it
// Submesh - Material combination
typedef struct VertexMaterial{
	VertexData* vertex_data;
	Material* material;
	std::vector<Uniform> uniform_changes;		// Temporary change to the uniforms of the materials shader
	//DrawSpecification draw_specs;				// TODO: tell the renderer, with which draw command and how this combination should be drawn on the screen

	VertexMaterial(VertexData* vd, Material* m, const std::vector<Uniform>& uc){
		id = ++id_count;
		vertex_data = vd;
		material = m;
		uniform_changes = uc;
	}
	VertexMaterial(const VertexMaterial& vm){
		*this = vm;
	}

	unsigned int get_id(){ return id; }

	// Sort by shader, then by material type, then by uniform change count
	bool operator<(const VertexMaterial& vm) const{
		// compare shaders (more precisely: their pointers)
		// no need for having a specific shader order (currently), just put them in a bunch
		if(material->get_shader() != vm.material->get_shader()){
			if(material->get_shader() < vm.material->get_shader()) return true;
			return false;
		}
		// equal shaders -> compare materials (more precisely: their pointers)
		if(material != vm.material){
			if(material < vm.material) return true;
			return false;
		}
		// equal materials -> compare uniform change count (least come first)
		if(uniform_changes.size() != vm.uniform_changes.size()){
			if(uniform_changes.size() < vm.uniform_changes.size()) return true;
			return false;
		}
		// use ID for proof of inequality
		return id < vm.id;
	}

	void operator=(const VertexMaterial& vm){
		id = ++id_count;
		vertex_data = vm.vertex_data;
		material = vm.material;
		uniform_changes = vm.uniform_changes;
	}

	struct less{
		bool operator()(VertexMaterial* vm1, VertexMaterial* vm2) const{
			if(!vm1 || !vm2) return vm1 < vm2;
			return *vm1 < *vm2;
		}
	};

private:
	static unsigned int id_count;
	unsigned int id;

} VertexMaterial;


}


#endif // !__VERTEX_MATERIAL_H__
