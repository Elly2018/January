#include "action.h"
#include <imgui.h>
#include <spdlog/spdlog.h>
#include "../../engine/engine.h"
#include "../../engine/struct/config.h"
#include "../../engine/struct/context.h"

namespace January::Engine::View {
    void JViewAction::Init(){
        spdlog::info("Loaded View: Action");
    }
    void JViewAction::Update(){

    }
    void JViewAction::Draw(){
        if(ImGui::Button("Play")){

        }
        ImGui::SameLine();
        if(ImGui::Button("Stop")){
            
        }

        ImGui::Text("FPS: %f", (1.0f / jengine.context->delta));
        ImGui::Text("Time: %f", jengine.context->time);
        ImGui::Text("DeltaTime: %f", jengine.context->delta);
    }
    void JViewAction::DeInit(){

    }

    void JViewAction::Play(){

    }
    void JViewAction::Pause(){

    }
    void JViewAction::Stop(){

    }
}