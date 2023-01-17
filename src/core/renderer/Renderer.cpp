#include "Renderer.h"

tse::Renderer* current_renderer = new tse::Renderer();

namespace tse{

Renderer::Renderer(){}
Renderer::~Renderer(){}

void Renderer::setup(){
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
void Renderer::clear(){
	// TODO: allow unique clear
	glClearColor(0.2, 0.8, 0.5, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Renderer::draw(){
	const auto& seq = IRenderManager->get_render_sequence();
	for(size_t i = 0; i < seq.size(); i++){
		seq.at(i)->render();
	}
}


}


