#include "project_dashboard.h"
#include <imgui.h>
#include <spdlog/spdlog.h>

namespace January::Engine::View {
    bool JPopupProjectDashboard::PreDraw() {
        window_flag = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 display = io.DisplaySize;
        ImVec2 unit = ImVec2(display.x / 10.0f, display.y / 10.0f);
        ImGui::SetNextWindowPos(ImVec2(unit.x * 0.5F, unit.y * 0.5F), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(unit.x * 9, unit.y * 9), ImGuiCond_Always);
        return JPopupBase::PreDraw();
    }

    void JPopupProjectDashboard::Draw() {
        
    }
}