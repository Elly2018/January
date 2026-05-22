#include "application.h"
#include <imgui.h>
#include <spdlog/spdlog.h>
#include "../../../engine/engine.h"
#include "../../../engine/struct/config.h"
#include "../../../engine/struct/context.h"

namespace January::Engine::View {
    void JViewApplication::Init(){
        spdlog::info("Loaded View: Application");
    }
    void JViewApplication::Update(){

    }
    void JViewApplication::Draw(){
        if(ImGui::Button("Play")){

        }
        ImGui::SameLine();
        if(ImGui::Button("Stop")){
            
        }

        ImGui::Text("FPS: %f", (1.0f / jengine.context->delta));
        ImGui::Text("Time: %f", jengine.context->time);
        ImGui::Text("DeltaTime: %f", jengine.context->delta);
    }
    void JViewApplication::DeInit(){

    }

    void JViewApplication::Play(){

    }
    void JViewApplication::Pause(){

    }
    void JViewApplication::Stop(){

    }
}