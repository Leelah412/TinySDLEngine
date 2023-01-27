#include "ModelManager.h"

namespace tse{

ModelManager* ModelManager::s_default = new ModelManager();

ModelManager::ModelManager(){}

ModelManager::~ModelManager(){
	delete m_vao;
	delete m_light_ubo;
}

/**********/
/* PUBLIC */
/**********/

void ModelManager::init_world(){
	if(!m_expected_max_vertices) return;
	if(!m_expected_max_indices) return;

	m_vao = new VertexArray();
	VertexAttributeLayout layout;
	layout.push(3, GL_FLOAT, false);		// Position
	layout.push(2, GL_FLOAT, false);		// UV1
	layout.push(3, GL_FLOAT, false);		// Normal
	//layout.push(2, GL_FLOAT, false);		// UV2

	m_vao->create_vertex_buffer(m_expected_max_vertices * layout.get_stride(), layout);
	m_vao->create_index_buffer(m_expected_max_indices);

	// Create intervals spanning entire buffers
	// VBO
	m_interval_vertex = {};
	m_vertex_interval = {};
	m_vbo_space = {};
	m_free_vbo_intervals = {};
	add_free_vbo_space( DisjointInterval(1, m_expected_max_vertices) );
	// IBO
	m_interval_index = {};
	m_index_interval = {};
	m_ibo_space = {};
	m_free_ibo_intervals = {};
	add_free_ibo_space( DisjointInterval(0, m_expected_max_indices - 1) );

	// Light UBO
	// TODO: implement dingens
	m_light_ubo = new UniformBuffer(0);
}

void ModelManager::add_model(Model* model){
	if(!model) return;
	// Model must have a Mesh
	if(!model->get_mesh()){
		std::cout << "WARNING: Trying to add a Model without a Mesh!" << std::endl;
		return;
	}
	if(m_models.find(model) != m_models.end()) return;

	// Add to lists
	m_models.insert(model);
	m_cur_submeshes[model] = {};
	auto gvm = model->get_vertex_materials();

	for(int i = 0; i < gvm.size(); i++){
		// Copy VertexMaterial, since original VM is located on stack!
		// The reason we do this instead of saving a simple reference is, that a VM could be removed from a Model,
		// before we could remove the ref from our lists here, meaning we won't know, if a VM in "m_world" would belong to the Model,
		// as the point of reference is completely gone.
		VertexMaterial* _vm = new VertexMaterial(nullptr, nullptr, {});
		auto& vm = gvm.at(i).get();
		std::memcpy(_vm, &vm, sizeof(VertexMaterial));
		_vm->uniform_changes = vm.uniform_changes;
		m_cur_submeshes[model].insert(_vm);
		m_world.insert(_vm);
	}

	const GLuint max_buffer_size = -1;										// max. allowed size per buffer
	GLuint free_vbo = get_free_vbo_space(),									// nr. of vertices we can insert to VBO
		   free_ibo = get_free_ibo_space();									// nr. of indices we can insert to IBO
	const VertexBuffer* vbo = m_vao->get_vertex_buffer();
	const IndexBuffer* ibo = m_vao->get_index_buffer();
	GLuint vertex_size = vbo->get_layout().get_stride();
	GLuint mesh_vertex_size = model->get_mesh()->mesh_vertex_size(),
		   mesh_index_count = model->get_mesh()->mesh_index_count();

	// Check, if mesh can fit into buffer
	// If no, increase buffer size and reallocate every Model currently in the world
	bool realloc_vbo = false, realloc_ibo = false;

	// VBO Realloc check
	if(mesh_vertex_size / vertex_size > free_vbo){

		// Allocate more buffer, if no free space available
		// Added size should be at least vertex size of models mesh + current buffer size + 6.25% of new buffer size, and at most 2^32 - 1 bytes
		unsigned long long new_size = (unsigned long long)mesh_vertex_size + vbo->get_size();
		// Check, if we can fit the model at all; if no, throw an error and abort everything
		if((new_size - (unsigned long long)free_vbo * vertex_size) > max_buffer_size){
			std::cerr << "ERROR: Can't assign model to buffer: Vertex Buffer size already at max." << std::endl;
			return;
		}
		// Try to add an additional amount, if possible
		new_size += new_size >> 4;
		new_size = std::min(new_size, (unsigned long long)max_buffer_size);

		realloc_vertex_buffer(new_size, vbo->is_static());
		realloc_vbo = true;
	}

	// IBO Realloc check
	if(mesh_index_count > free_ibo){

		// Allocate more buffer, if no free space available
		// Added count should be at least index size of models mesh + current buffer size + 6.25% of new buffer size, and at most 2^32 - 1 bytes
		unsigned long long new_count = (unsigned long long)mesh_index_count + ibo->get_max_indices();
		// Check, if we can fit the model at all; if no, throw an error and abort everything
		if((new_count - free_ibo) * sizeof(GLuint) > max_buffer_size){
			std::cerr << "ERROR: Can't assign model to buffer: Vertex Buffer size already at max." << std::endl;
			return;
		}
		// Try to add an additional amount, if possible
		new_count += new_count >> 4;
		new_count = std::min(new_count, (unsigned long long)(max_buffer_size / sizeof(GLuint)));

		realloc_index_buffer(new_count, ibo->is_static());
		realloc_ibo = true;
	}

	// Push meshes into buffer, as long as there was no reallocation
	std::multimap<GLuint, DisjointInterval>::iterator iv;		// for getting interval to push meshes into

	// Vertex Buffer
	if(!realloc_vbo){
		for(auto vm : m_cur_submeshes[model]){
			// Since we are here, it means entire mesh could fit into buffer, so if we can't find space for a submesh,
			// then this is the right "opportunity" to left-shift the buffer memory to make space for new elements
			if((iv = find_ideal_vertex_interval(vm->vertex_data->get_vertex_array_size() / vertex_size))
				== m_vbo_space.end()){
				left_shift_vertex_buffer();

				// If we can't find an ideal interval again, there must be some kind of problem with... everything
				if((iv = find_ideal_vertex_interval(vm->vertex_data->get_vertex_array_size() / vertex_size))
					== m_vbo_space.end()){
					std::cerr << "ERROR: Buffer is big enough and has been shifted left, but we still can't find space for the mesh?!?!" << std::endl;
					return;
				}
			}
			// Finally add the submesh into the vertex buffer
			// Interval can be the found one, even if it's bigger than needed, the function will handle that
			move_vertex_materials_in_vbo(vm, iv->second);
		}
	}

	// Index Buffer
	if(!realloc_ibo){
		for(auto vm : m_cur_submeshes[model]){
			// Since we are here, it means entire mesh could fit into buffer, so if we can't find space for a submesh,
			// then this is the right "opportunity" to left-shift the buffer memory to make space for new elements
			if(( iv = find_ideal_index_interval(vm->vertex_data->get_indices().size()) )
				== m_ibo_space.end()){
				left_shift_index_buffer();

				// If we can't find an ideal interval again, there must be some kind of problem with... everything
				if(( iv = find_ideal_index_interval(vm->vertex_data->get_indices().size()) )
					== m_ibo_space.end()){
					std::cerr << "ERROR: Buffer is big enough and has been shifted left, but we still can't find space for the mesh?!?!" << std::endl;
					return;
				}
			}
			// Finally add the submesh into the vertex buffer
			// Interval can be the found one, even if it's bigger than needed, the function will handle that
			move_vertex_materials_in_ibo(vm, iv->second);
		}
	}
}

void ModelManager::update_model(Model* model){
	// Easiest way to update a model -> remove, then add it back
	remove_model(model);
	add_model(model);
}

void ModelManager::remove_model(Model* model){
	if(!model) return;
	m_models.erase(model);

	// TODO: Remove from buffer

	// Remove from list
	if(m_cur_submeshes.find(model) == m_cur_submeshes.end()) return;
	VertexMaterial* vm;
	auto vm_it = m_cur_submeshes[model].begin();
	for(vm_it; vm_it != m_cur_submeshes[model].end(); vm_it++){
		m_world.erase(*vm_it);
	}
	m_cur_submeshes.erase(model);
}

const std::set<Model*>& ModelManager::get_models() const{
	return m_models;
}

const std::set<VertexMaterial*, VertexMaterial::less>& ModelManager::get_world() const{
	return m_world;
}

VertexArray* ModelManager::get_vertex_array() const{
	return m_vao;
}

const DisjointInterval& ModelManager::get_vertex_buffer_interval(VertexMaterial* vm) const{
	auto di = m_vertex_interval.find(vm);
	// use -1 (i.e. highest unsigned int value) to represent non-existence of VM
	if(di == m_vertex_interval.end()) return DisjointInterval(-1, -1);
	return di->second;
}

const DisjointInterval& ModelManager::get_index_buffer_interval(VertexMaterial* vm) const{
	auto di = m_index_interval.find(vm);
	// use -1 (i.e. highest unsigned int value) to represent non-existence of VM
	if(di == m_index_interval.end()) return DisjointInterval(-1, -1);
	return di->second;
}

ModelManager* ModelManager::get_default_model_manager(){
	return s_default;
}

/***********/
/* PRIVATE */
/***********/

std::multimap<GLuint, DisjointInterval>::iterator ModelManager::find_ideal_vertex_interval(GLuint count){
	// no space
	if((*m_vbo_space.rbegin()).first < count) return m_vbo_space.end();
	// found space fitting perfectly for given count
	std::multimap<GLuint, DisjointInterval>::iterator it;
	if((it = m_vbo_space.find(count)) != m_vbo_space.end()) return it;
	if((it = m_vbo_space.upper_bound(count)) != m_vbo_space.end()) return it;

	// somehow found nothing?!? this shouldn't be possible
	std::cout << "Logically impossible point reached, while trying to find ideal vertex interval" << std::endl;
	return m_vbo_space.end();
}

std::multimap<GLuint, DisjointInterval>::iterator ModelManager::find_ideal_index_interval(GLuint count){
	// no space
	if((*m_ibo_space.rbegin()).first < count) return m_ibo_space.end();
	// found space fitting perfectly for given count
	std::multimap<GLuint, DisjointInterval>::iterator it;
	if((it = m_ibo_space.find(count)) != m_ibo_space.end()) return it;
	if((it = m_ibo_space.upper_bound(count)) != m_ibo_space.end()) return it;

	// somehow found nothing?!? this shouldn't be possible
	std::cout << "Logically impossible point reached, while trying to find ideal index interval" << std::endl;
	return m_ibo_space.end();
}

void ModelManager::realloc_vertex_buffer(GLuint size, bool is_static){
	if(!size){
		std::cout << "WARNING: Tried to reallocate 0 bytes for vertex buffer! Abort." << std::endl;
		return;
	}

	// reset intervals and allocate buffer
	m_vertex_interval = {};
	m_interval_vertex = {};
	m_free_vbo_intervals = {};
	m_vbo_space = {};

	m_vao->alloc_vertex_buffer(size, is_static);
	GLuint offset = 1;
	GLuint vertex_size = m_vao->get_vertex_buffer()->get_layout().get_stride();
	if(!vertex_size){
		std::cerr << "ERROR: Vertex attribute size is 0 bytes! Abort!" << std::endl;
		return;
	}

	// set free space first
	add_free_vbo_space(DisjointInterval(offset, offset + size / vertex_size));

	// insert models
	for(auto vm : m_world){
		move_vertex_materials_in_vbo(vm, DisjointInterval(offset, offset - 1 + vm->vertex_data->get_vertex_array_size() / vertex_size));
		offset += vm->vertex_data->get_vertex_array_size() / vertex_size;
	}
}

void ModelManager::realloc_index_buffer(GLuint count, bool is_static){
	if(!count){
		std::cout << "WARNING: Tried to reallocate 0 bytes for index buffer! Abort." << std::endl;
		return;
	}

	// reset intervals and allocate buffer
	m_index_interval = {};
	m_interval_vertex = {};
	m_free_ibo_intervals = {};
	m_ibo_space = {};

	m_vao->alloc_index_buffer(count, is_static);
	GLuint offset = 0;

	// set free space first
	add_free_ibo_space(DisjointInterval(offset, offset + count));

	// insert models
	for(auto vm : m_world){
		move_vertex_materials_in_ibo(vm, DisjointInterval(offset, offset - 1 + vm->vertex_data->get_indices().size()));
		offset += vm->vertex_data->get_indices().size();
	}
}

void ModelManager::left_shift_vertex_buffer(){

	// concatenate free spaces first to ensure correctness
	concat_free_vbo_space();

	GLuint vertex_size = m_vao->get_vertex_buffer()->get_layout().get_stride();
	if(!vertex_size){
		std::cerr << "ERROR: Vertex attributes have size of 0!" << std::endl;
		return;
	}
	// for each free space between two occupied spaces, shift latter elements to the left
	std::set<DisjointInterval>::iterator it = m_free_vbo_intervals.begin(), it_next;
	for(it; it != m_free_vbo_intervals.end(); it++){
		it_next = std::next(it, 1);

		unsigned int start_goal = it->start;			// until where we want to push elements
		unsigned int start_el = it->end + 1;			// position of first element we want to push
		unsigned int end_el;							// position of last element we want to push

		if(it_next != m_free_vbo_intervals.end())
			end_el = it_next->start - 1;
		else
			end_el = m_vao->get_vertex_buffer()->get_size() / vertex_size - 1;

		// get all elements in range
		std::set<VertexMaterial*> mats = get_vertex_materials_in_vbo_range(DisjointInterval(start_el, end_el));

		// TODO: this might be inefficient, but whatever
		// maybe even just allocate entirely new buffer instead of doing this, if possible
		// but for now we do it like this
		unsigned int offset = start_goal;
		for(VertexMaterial* vm : mats){
			if(!vm->vertex_data->get_vertex_array_size()){
				std::cerr << "ERROR: somehow have a submesh with size 0" << std::endl;
				continue;
			}
			// free space interval also handled in function, don't worry about that
			move_vertex_materials_in_vbo(vm, DisjointInterval(offset, offset - 1 + vm->vertex_data->get_vertex_array_size() / vertex_size));
			offset += vm->vertex_data->get_vertex_array_size() / vertex_size;
		}
	}

}

void ModelManager::left_shift_index_buffer(){
	// concatenate free spaces first to ensure correctness
	concat_free_ibo_space();

	// for each free space between two occupied spaces, shift latter elements to the left
	std::set<DisjointInterval>::iterator it = m_free_ibo_intervals.begin(), it_next;
	for(it; it != m_free_ibo_intervals.end(); it++){
		it_next = std::next(it, 1);

		unsigned int start_goal = it->start;			// until where we want to push elements
		unsigned int start_el = it->end + 1;			// position of first element we want to push
		unsigned int end_el;							// position of last element we want to push

		if(it_next != m_free_ibo_intervals.end())
			end_el = it_next->start - 1;
		else
			end_el = m_vao->get_index_buffer()->get_max_indices();

		// get all elements in range
		std::set<VertexMaterial*> mats = get_vertex_materials_in_ibo_range(DisjointInterval(start_el, end_el));

		// TODO: this might be inefficient, but whatever
		// maybe even just allocate entirely new buffer instead of doing this, if possible
		// but for now we do it like this
		unsigned int offset = start_goal;
		for(VertexMaterial* vm : mats){
			if(!vm->vertex_data->get_indices().size()){
				// TODO: handle this differently
				std::cerr << "ERROR: somehow have a submesh with size 0" << std::endl;
				continue;
			}
			// free space interval also handled in function, don't worry about that
			move_vertex_materials_in_ibo(vm, DisjointInterval(offset, offset - 1 + vm->vertex_data->get_indices().size()));
			offset += vm->vertex_data->get_indices().size();
		}
	}
}

GLuint ModelManager::get_free_vbo_space(){
	GLuint count = 0;
	for(auto s : m_vbo_space){
		count += s.first;
	}
	return count;
}

GLuint ModelManager::get_free_ibo_space(){
	GLuint count = 0;
	for(auto s : m_ibo_space){
		count += s.first;
	}
	return count;
}

// TODO: check, that things really run smoothly
void ModelManager::add_free_vbo_space(const DisjointInterval& di){
	DisjointInterval cur_di = di;

	// check lower bounds
	std::set<DisjointInterval>::iterator it = m_free_vbo_intervals.lower_bound(cur_di);
	if(it != m_free_vbo_intervals.end() && (it->end + 1 >= cur_di.start)){
		// exact same interval already exists
		if(it->start == cur_di.start && (it->end == cur_di.end)) return;
		// new interval is inside old interval
		if(it->end >= cur_di.end) return;
		// intervals overlap, but one doesn't contain the other -> concat
		cur_di = DisjointInterval(it->start, cur_di.end);
		remove_free_vbo_space(*it);
	}

	// check upper bounds
	it = m_free_vbo_intervals.upper_bound(cur_di);
	if(it != m_free_vbo_intervals.end() && (it->start <= cur_di.end + 1)){
		// exact same interval already exists
		if(it->start == cur_di.start && (it->end == cur_di.end)) return;
		// intervals overlap, but one doesn't contain the other -> concat
		cur_di = DisjointInterval(cur_di.start, it->end);
		remove_free_vbo_space(*it);
	}

	m_free_vbo_intervals.insert(cur_di);
	m_vbo_space.insert(std::pair<GLuint, DisjointInterval>(cur_di.end - cur_di.start + 1, cur_di));
}

// TODO: check, that things really run smoothly
void ModelManager::add_free_ibo_space(const DisjointInterval& di){
	DisjointInterval cur_di = di;

	// check lower bounds
	std::set<DisjointInterval>::iterator it = m_free_ibo_intervals.lower_bound(cur_di);
	if(it != m_free_ibo_intervals.end() && (it->end + 1 >= cur_di.start)){
		// exact same interval already exists
		if(it->start == cur_di.start && (it->end == cur_di.end)) return;
		// new interval is inside old interval
		if(it->end >= cur_di.end) return;
		// intervals overlap, but one doesn't contain the other -> concat
		cur_di = DisjointInterval(it->start, cur_di.end);
		remove_free_ibo_space(*it);
	}

	// check upper bounds
	it = m_free_ibo_intervals.upper_bound(cur_di);
	if(it != m_free_ibo_intervals.end() && (it->start <= cur_di.end + 1)){
		// exact same interval already exists
		if(it->start == cur_di.start && (it->end == cur_di.end)) return;
		// intervals overlap, but one doesn't contain the other -> concat
		cur_di = DisjointInterval(cur_di.start, it->end);
		remove_free_ibo_space(*it);
	}

	m_free_ibo_intervals.insert(di);
	m_ibo_space.insert(std::pair<GLuint, DisjointInterval>(di.end - di.start + 1, di));
}

void ModelManager::remove_free_vbo_space(const DisjointInterval& di){
	// check, if interval actually exists (less operator of DI only compares start values)
	auto it = m_free_vbo_intervals.find(di);
	if(it == m_free_vbo_intervals.end()) return;
	if(it->end != di.end) return;

	// copying by reference, so erasing "di" might invalidate it, so create new disjoint interval
	DisjointInterval _di = DisjointInterval(di.start, di.end);
	m_free_vbo_intervals.erase(di);

	// remove from vbo space too
	unsigned int size = _di.end - _di.start + 1;
	if(m_vbo_space.find(size) == m_vbo_space.end()) return;
	for(std::multimap<GLuint, DisjointInterval>::iterator it = m_vbo_space.lower_bound(size); it != m_vbo_space.upper_bound(size); it++){
		if(it->second.start == _di.start && (it->second.end == _di.end)){
			m_vbo_space.erase(it);
			break;
		}
	}
}

void ModelManager::remove_free_ibo_space(const DisjointInterval& di){
	// check, if interval actually exists (less operator of DI only compares start values)
	auto it = m_free_ibo_intervals.find(di);
	if(it == m_free_ibo_intervals.end()) return;
	if(it->end != di.end) return;

	// copying by reference, so erasing "di" might invalidate it, so create new disjoint interval
	DisjointInterval _di = DisjointInterval(di.start, di.end);
	m_free_ibo_intervals.erase(di);

	unsigned int size = _di.end - _di.start + 1;
	if(m_ibo_space.find(size) == m_ibo_space.end()) return;
	for(std::multimap<GLuint, DisjointInterval>::iterator it = m_ibo_space.lower_bound(size); it != m_ibo_space.upper_bound(size); it++){
		if(it->second.start == _di.start && (it->second.end == _di.end)){
			it = m_ibo_space.erase(it);
			break;
		}
	}
}

std::set<VertexMaterial*> ModelManager::get_vertex_materials_in_vbo_range(const DisjointInterval& di){
	DisjointInterval start = di, end = DisjointInterval(di.end + 1, di.end + 1);
	std::set<VertexMaterial*> mats = {};
	for(std::map<DisjointInterval, VertexMaterial*>::iterator it = m_interval_vertex.lower_bound(di); it != m_interval_vertex.upper_bound(end); it++){
		mats.insert(it->second);
	}
	return mats;
}

std::set<VertexMaterial*> ModelManager::get_vertex_materials_in_ibo_range(const DisjointInterval& di){
	DisjointInterval start = di, end = DisjointInterval(di.end + 1, di.end + 1);
	std::set<VertexMaterial*> mats = {};
	for(std::map<DisjointInterval, VertexMaterial*>::iterator it = m_interval_index.lower_bound(di); it != m_interval_index.upper_bound(end); it++){
		mats.insert(it->second);
	}
	return mats;
}

void ModelManager::move_vertex_materials_in_vbo(VertexMaterial* vm, const DisjointInterval& new_iv){

	// SPACE CHECK

	unsigned int vertex_size = m_vao->get_vertex_buffer()->get_layout().get_stride();
	unsigned int vm_vertex_count = vm->vertex_data->get_vertex_array_size() / vertex_size;
	unsigned int new_iv_size = new_iv.end - new_iv.start + 1;
	DisjointInterval used_iv = new_iv;
	std::set<DisjointInterval>::iterator dj_it = m_free_vbo_intervals.lower_bound(new_iv);
	// return, if no space exists
	if(dj_it == m_free_vbo_intervals.end() ||
		(dj_it != m_free_vbo_intervals.end() && new_iv.end > dj_it->end)) return;

	// vm doesn't even fit into desired interval -> output error and return 
	if(new_iv_size < vm_vertex_count){
		std::cout << "WARNING: Trying to move VertexMaterial into too small vertex interval." << std::endl;
		return;
	}
	// interval is bigger than needed -> reduce size and left-align occupied interval 
	else if(new_iv_size > vm_vertex_count){
		used_iv = DisjointInterval(new_iv.start, new_iv.start + vm_vertex_count - 1);
	}

	// remove old entries, if they exist or if vertex count is somehow zero
	std::map<VertexMaterial*, DisjointInterval>::iterator it = m_vertex_interval.find(vm);
	if(it != m_vertex_interval.end()){
		DisjointInterval i = it->second;
		m_interval_vertex.erase(i);
		m_vertex_interval.erase(it);
		// add free space into the exact positions
		add_free_vbo_space(i);
		if(!vm_vertex_count){
			std::cout << "WARNING: Trying to assign vertex buffer space to VertexMaterial with no vertices!" << std::endl;
			return;
		}
	}
	else if(!vm_vertex_count){
		std::cout << "WARNING: Trying to assign vertex buffer space to VertexMaterial with no vertices!" << std::endl;
		return;
	}

	// insert new entry
	m_vertex_interval.insert(std::pair<VertexMaterial*, DisjointInterval>(vm, used_iv));
	m_interval_vertex.insert(std::pair<DisjointInterval, VertexMaterial*>(used_iv, vm));
	m_vao->push_vertex_buffer(vm->vertex_data->get_vertices(), used_iv.start * vertex_size, (used_iv.end - used_iv.start + 1) * vertex_size);

	// remove old free space interval, into which vm is moved, and create new ones, if possible
	DisjointInterval old = *dj_it;
	remove_free_vbo_space(*dj_it);
	if(used_iv.end != old.end)
		add_free_vbo_space(DisjointInterval(used_iv.end + 1, old.end));
	if(used_iv.start != old.start)
		add_free_vbo_space(DisjointInterval(old.start, used_iv.start - 1));
}

void ModelManager::move_vertex_materials_in_ibo(VertexMaterial* vm, const DisjointInterval& new_iv){

	// SPACE CHECK

	unsigned int vm_index_count = vm->vertex_data->get_indices().size();
	unsigned int new_iv_size = new_iv.end - new_iv.start + 1;
	DisjointInterval used_iv = new_iv;
	std::set<DisjointInterval>::iterator dj_it = m_free_ibo_intervals.lower_bound(new_iv);
	// return, if no space exists
	if(dj_it == m_free_ibo_intervals.end() ||
		(dj_it != m_free_ibo_intervals.end() && new_iv.end > dj_it->end)) return;

	// vm doesn't even fit into desired interval -> output error and return 
	if(new_iv_size < vm_index_count){
		std::cout << "WARNING: Trying to move VertexMaterial into too small index interval." << std::endl;
		return;
	}
	// interval is bigger than needed -> reduce size and left-align occupied interval 
	else if(new_iv_size > vm_index_count){
		used_iv = DisjointInterval(new_iv.start, new_iv.start + vm_index_count - 1);
	}

	// remove old entries, if they exist
	std::map<VertexMaterial*, DisjointInterval>::iterator it = m_index_interval.find(vm);
	if(it != m_index_interval.end()){
		// nullify index buffer values
		m_vao->pop_index_buffer(it->second.start, it->second.end - it->second.start + 1);
		DisjointInterval i = it->second;
		m_interval_index.erase(i);
		m_index_interval.erase(it);
		// add free space into the exact positions
		add_free_ibo_space(i);
		if(!vm_index_count){
			std::cout << "WARNING: Trying to assign index buffer space to VertexMaterial with no indices!" << std::endl;
			return;
		}
	}
	else if(!vm_index_count){
		std::cout << "WARNING: Trying to assign index buffer space to VertexMaterial with no indices!" << std::endl;
		return;
	}

	// insert new entry
	m_index_interval.insert(std::pair<VertexMaterial*, DisjointInterval>(vm, used_iv));
	m_interval_index.insert(std::pair<DisjointInterval, VertexMaterial*>(used_iv, vm));
	m_vao->push_index_buffer((const void*)vm->vertex_data->get_indices().data(), used_iv.start, used_iv.end - used_iv.start + 1);

	// remove old free space interval, into which vm is moved, and create new ones, if possible
	DisjointInterval old = *dj_it;
	remove_free_ibo_space(*dj_it);
	if(used_iv.end != old.end)
		add_free_ibo_space(DisjointInterval(used_iv.end + 1, old.end));
	if(used_iv.start != old.start)
		add_free_ibo_space(DisjointInterval(old.start, used_iv.start - 1));
}

void ModelManager::concat_free_vbo_space(){
	std::set<DisjointInterval>::iterator it = m_free_vbo_intervals.begin(), it_next = std::next(it, 1);
	for(it; it_next != m_free_vbo_intervals.end();){
		if(it->end + 1 >= it_next->start){
			// next interval is inside current one somehow -> just remove next interval
			if(it->end >= it_next->end){
				remove_free_vbo_space(*it_next);
				// don't increment "it", we still want to check it with the interval after "it_next"
			}
			else{
				DisjointInterval cnct = DisjointInterval(it->start, it_next->end);
				remove_free_vbo_space(*it);
				remove_free_vbo_space(*it_next);
				add_free_vbo_space(cnct);
				it = m_free_vbo_intervals.find(cnct);
			}
		}
		else{
			it++;
		}
		it_next = std::next(it, 1);
	}
}

void ModelManager::concat_free_ibo_space(){
	std::set<DisjointInterval>::iterator it = m_free_ibo_intervals.begin(), it_next = std::next(it, 1);
	for(it; it_next != m_free_ibo_intervals.end();){
		if(it->end + 1 >= it_next->start){
			// next interval is inside current one somehow -> just remove next interval
			if(it->end >= it_next->end){
				remove_free_ibo_space(*it_next);
				// don't increment "it", we still want to check it with the interval after "it_next"
			}
			else{
				DisjointInterval cnct = DisjointInterval(it->start, it_next->end);
				remove_free_ibo_space(*it);
				remove_free_ibo_space(*it_next);
				add_free_ibo_space(cnct);
				it = m_free_ibo_intervals.find(cnct);
			}
		}
		else{
			it++;
		}
		it_next = std::next(it, 1);
	}
}

void ModelManager::calc_expected_max_vertices_indices(){
	m_expected_max_vertices = 0xFFFFFF;
	m_expected_max_indices = 0xFFFFFF;
}


}