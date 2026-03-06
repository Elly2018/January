#include "nodebase.h"
#include <imgui.h>
#include <imgui_node_editor.h>

namespace ed = ax::NodeEditor;

namespace January::Engine::Node {
    NodeBase::NodeBase(){

    }
    NodeBase::~NodeBase(){
        
    }

    void NodeBase::PreDraw(int32_t& uniqueId){
        uniqueId++;
        ed::BeginNode(uniqueId++);
        ImGui::Text("Node A");
    }

    void NodeBase::Draw(){
        
    }

    void NodeBase::PostDraw(){
        ed::EndNode();
    }

}