#include "ToolbarWidget.h"

#define TOOLBAR_HEIGHT 32
#define TOOLBAR_FLAGS ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration

namespace tse{

ToolbarWidget::ToolbarWidget() : Widget(0, 0, Config::window_w, TOOLBAR_HEIGHT){
}

ToolbarWidget::~ToolbarWidget(){
}

void ToolbarWidget::draw(){
    if(ImGui::Begin("Toolbar", &m_show, TOOLBAR_FLAGS)){
        if(ImGui::BeginMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
                if(ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End();
    }
}

}