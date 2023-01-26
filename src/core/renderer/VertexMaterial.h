#ifndef __VERTEX_MATERIAL_H__
#define __VERTEX_MATERIAL_H__

#include "Material.h"
#include "VertexData.h"

namespace tse{

// Represents the general combination of VBO/IBO data and the material applied to it
// Submesh - Material combination
typedef struct VertexMaterial{
	// Since it could be possible, that more than one Model have the same Mesh and Material,
	// we need a way to differentiate between the VMs of each Model, as otherwise, each Mesh - Material combination would only get rendered once.
	// If not needed, just let "id" stay at 0.
	unsigned int id = 0;
	VertexData* vertex_data;
	Material* material;
	std::vector<Uniform> uniform_changes;		// Temporary change to the uniforms of the materials shader
	//DrawSpecification draw_specs;				// TODO: tell the renderer, with which draw command and how this combination should be drawn on the screen

	VertexMaterial(VertexData* vd, Material* m, const std::vector<Uniform>& uc){
		vertex_data = vd;
		material = m;
		uniform_changes = uc;
	}

	// Sort by shader, then by material type, then by uniform change count
	bool operator<(const VertexMaterial& vm) const{
		// one or both have no materials
		if(!material){
			if(!vm.material){
				// use "vertex_data" pointer to determine (in)equality
				return vertex_data < vm.vertex_data;
			}
			// the null ones will get the default shader -> push them forward!
			return true;
		}
		else if(!vm.material){
			return false;
		}

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
			// specifically compare uniform changes
			// O(m*n), but it can be assumed, that there won't be many temporary uniform changes anyway, so it's fine
			bool found_equivalence;
			Uniform last_uni, last_vm_uni;
			for(auto& uni : uniform_changes){
				found_equivalence = false;
				for(auto& vm_uni : vm.uniform_changes){
					if(uni == vm_uni){
						found_equivalence = true;
						break;
					}
					last_uni = uni;
					last_vm_uni = vm_uni;
				}
				if(!found_equivalence){
					return last_uni < last_vm_uni;
				}

			}
		}

		// check, if same vertex data
		if(vertex_data != vm.vertex_data) return vertex_data < vm.vertex_data;

		// use "id" to determine (in)equality, in case the previous ones couldn't
		return id < vm.id;
	}

	struct less{
		bool operator()(VertexMaterial* vm1, VertexMaterial* vm2) const{
			if(!vm1 || !vm2) return vm1 < vm2;
			return *vm1 < *vm2;
		}
	};

} VertexMaterial;


}


#endif // !__VERTEX_MATERIAL_H__
