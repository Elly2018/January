#include "explorer.h"
#include <filesystem>
#include <spdlog/spdlog.h>
#include <imgui.h>
#include "../../engine.h"

namespace fs = std::filesystem;

// Global engine access point
extern JEngine jengine;

JViewExplorer::JViewExplorer(const char* _title, int32_t _type) : JViewBase(_title, _type) {

}

JViewExplorer::~JViewExplorer(){
    
}

void JViewExplorer::Init() {
    spdlog::info("Loaded View: Explorer");
}

void JViewExplorer::Update() {
    if(changed){
        changed = false;
    }
}

void JViewExplorer::Draw() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::Begin(title);
        if(!init){
            changed = true;
            init = true;
        }

        float w = ImGui::GetWindowWidth();
        leftWidth = w * (1.f / 3.f);
        rightWidth = w * (2.f / 3.f);

        AppContext* context = jengine.context.get();
        if(context == nullptr){
            spdlog::error("context is nullptr");
        }

        if(context != nullptr && fs::exists(context->project_path)){
            bool change = ImGui::InputText("Path", path, 512, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);
            if(change) changed = true;
            ImGui::BeginChild("ViewExplorer_Left", ImVec2(leftWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true);
                DrawLeftSide();
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild("ViewExplorer_Right", ImVec2(rightWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true);
                DrawRightSide();
            ImGui::EndChild();
        }else{
            ImGui::Text("You need to choose path fist");
        }
    ImGui::End();
}

void JViewExplorer::DeInit() {

}

void JViewExplorer::DrawLeftSide(){
    ImGui::Text("Left");
}

void JViewExplorer::DrawRightSide(){
    ImGui::Text("Right");
}