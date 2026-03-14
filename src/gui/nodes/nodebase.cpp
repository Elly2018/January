#include "nodebase.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <imgui.h>
#include <imgui_node_editor.h>

namespace ed = ax::NodeEditor;

namespace January::Engine::Node {
    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    NodeBase::NodeBase(NodeType _type, std::string _title, std::string _path){
        node_type = _type;
        title = _title;
        paths = split(_path, '/');
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