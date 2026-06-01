#include "application.h"
#include <atomic>
#define IMGUI_DEFINE_MATH_OPERATORS
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
            jengine.context->runtime_state.store(true, std::memory_order_release);
            RuntimeStart(*jengine.context);
        }
        ImGui::SameLine();
        if(ImGui::Button("Stop")){
            jengine.context->runtime_state.store(false, std::memory_order_release);
        }

        double delta_ms = jengine.context->delta.load(std::memory_order_relaxed);
        double fps = (delta_ms > 0.0) ? (1000.0 / delta_ms) : 0.0;

        double runtime_delta_ms = jengine.context->runtime_delta.load(std::memory_order_relaxed);
        double runtime_fps = (runtime_delta_ms > 0.0) ? (1000.0 / runtime_delta_ms) : 0.0;

        ImGui::Text("Engine State");
        ImGui::Text("FPS: %.2f frame/sec", fps);
        ImGui::Text("Time: %.2f sec", jengine.context->time.load(std::memory_order_relaxed) / 1000.0);
        ImGui::Text("DeltaTime: %.4f ms", jengine.context->delta.load(std::memory_order_relaxed));
        ImGui::Separator();
        ImGui::Text("Runtime State");
        ImGui::Text("FPS: %.2f frame/sec", runtime_fps);
        ImGui::Text("Time: %.2f sec", jengine.context->runtime_time.load(std::memory_order_relaxed) / 1000.0);
        ImGui::Text("DeltaTime: %.4f ms", jengine.context->runtime_delta.load(std::memory_order_relaxed));
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