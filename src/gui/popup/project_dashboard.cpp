#include "project_dashboard.h"
#include <imgui.h>
#include <spdlog/spdlog.h>

namespace January::Engine::View {
    bool JPopupProjectDashboard::PreDraw() {
        window_flag = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 display = io.DisplaySize;
        ImVec2 unit = ImVec2(display.x / 10.0f, display.y / 10.0f);
        ImGui::SetNextWindowPos(ImVec2(unit.x * 1, unit.y * 1), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(unit.x * 8, unit.y * 8), ImGuiCond_Always);
        return JPopupBase::PreDraw();
    }

    void JPopupProjectDashboard::Draw() {
        
    }
}