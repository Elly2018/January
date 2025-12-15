#include "explorer.h"
#include <spdlog/spdlog.h>
#include <imgui.h>

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
        float w = ImGui::GetWindowWidth();
        float leftWidth = w / 2.f;
        float rightWidth = w / 2.f;

        bool change = ImGui::InputText("Path", path, 512, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);
        if(change) changed = true;

        ImGui::BeginChild("ViewExplorer_Left", ImVec2(leftWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true);
            ImGui::Text("YOYOYO");
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("ViewExplorer_Right", ImVec2(rightWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true);
            ImGui::Text("YOYOYO22");
        ImGui::EndChild();
    ImGui::End();
}

void JViewExplorer::DeInit() {

}