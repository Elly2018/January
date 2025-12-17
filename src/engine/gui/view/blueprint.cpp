#include "blueprint.h"
#include <spdlog/spdlog.h>
#include <imgui.h>
#include "../../engine.h"

// Global engine access point
extern JEngine jengine;

JViewBlueprint::JViewBlueprint(const char* _title, int32_t _type) : JViewBase(_title, _type) {

}

JViewBlueprint::~JViewBlueprint(){

}

void JViewBlueprint::Init(){
    spdlog::info("Loaded View: Blueprint");
}
void JViewBlueprint::Update(){

}
void JViewBlueprint::Draw(){
    ImGui::Begin(title);
    
    ImGui::End();
}
void JViewBlueprint::DeInit(){

}