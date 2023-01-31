#include "SceneTreeWidget.h"

namespace tse{

SceneTreeWidget::SceneTreeWidget(){}
SceneTreeWidget::~SceneTreeWidget(){}

void SceneTreeWidget::set_root(Node* root){
	m_root = root;
	update_widget();
}

void SceneTreeWidget::update_widget(){

}

void SceneTreeWidget::draw(){
    if(ImGui::Begin("Scene Tree", &m_show)){
        ImGui::Text("Text");

        ImGui::End();
    }
}

}