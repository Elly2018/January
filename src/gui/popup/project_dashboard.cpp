#include "project_dashboard.h"
#include <filesystem>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <spdlog/spdlog.h>
#include <ini.h>
#include "../../engine/engine.h"
#include "../../engine/struct/config.h"
#include "../../engine/struct/context.h"
#include "../../engine/utility/command.h"
#include "../../engine/utility/path.h"

namespace fs = std::filesystem;

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

    void JPopupProjectDashboard::OnEnable(){
        JPopupBase::OnEnable();
        page = PAGE::MAIN;
    }

    void JPopupProjectDashboard::OnDisable(){
        JPopupBase::OnDisable();
    }

    void JPopupProjectDashboard::Init() {
        JPopupBase::Init();
    }

    void JPopupProjectDashboard::Update() {
        
    }

    void JPopupProjectDashboard::Draw() {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 winsize = ImGui::GetWindowSize();
        winsize.x -= (style.WindowPadding.x + style.FramePadding.x);
        winsize.y -= (style.WindowPadding.y + style.FramePadding.y);
        float bottom = ImGui::GetFrameHeightWithSpacing() * 3.5F;

        if(page == PAGE::MAIN){
            if(ImGui::BeginChild("Option##project_dashboard", ImVec2(((winsize.x / 5) * 1) - (style.ItemSpacing.x / 2), winsize.y - bottom), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoNav)){
                DrawOption();
                ImGui::EndChild();
            }
            ImGui::SameLine();
            if(ImGui::BeginChild("Recent##project_dashboard", ImVec2(((winsize.x / 5) * 4) - (style.ItemSpacing.x / 2), winsize.y - bottom), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoNav)){
                DrawRecent();
                ImGui::EndChild();
            }
            if(ImGui::Button("Cancel##project_dashboard")){
                SetEnable(false);
            }
        }else if(page == PAGE::NEW){
            if(ImGui::BeginChild("New##project_dashboard", ImVec2(winsize.x, winsize.y - bottom), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoNav)){
                DrawNewProject();
                ImGui::EndChild();
            }
            if(ImGui::Button("Confirm##project_dashboard")){
                GenerateProject();
            }
            ImGui::SameLine();
            if(ImGui::Button("Return##project_dashboard")){
                page = PAGE::MAIN;
            }
        }
    }

    void JPopupProjectDashboard::DrawOption(){
        if(ImGui::Button("New Project##project_dashboard_option")){
            p_name = "Default Project";
            p_path = get_temp_directory();
            page = PAGE::NEW;
        }
    }

    void JPopupProjectDashboard::DrawRecent(){
        for(auto& r : jengine.config->j_recent){
            if(ImGui::Selectable((r.j_path + "##project_dashboard_recent").c_str())){

            }
            if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()){

            }
        }
    }

    void JPopupProjectDashboard::DrawNewProject(){
        ImGui::InputText("Project Name##project_dashboard_new", &p_name);
        ImGui::InputText("Save Path##project_dashboard_new", &p_path);
        fs::path r = p_path;
        r /= p_name;
        ImGui::LabelText("Result Path##project_dashboard_new", r.c_str());
        if(ImGui::BeginCombo("Template##project_dashboard_new", GetTemplateName(temp).c_str())){
            for(int32_t i = 0; i < 6; i++){
                if(ImGui::Selectable((GetTemplateName((TEMPLATE)i) + "##project_dashboard_temps").c_str())){
                    temp = (TEMPLATE)i;
                }
            }
            ImGui::EndCombo();
        }
    }

    std::string JPopupProjectDashboard::GetTemplateName(TEMPLATE _temp){
        switch(_temp){
            default:
            case TEMPLATE::BLANK: return "Blank";
            case TEMPLATE::RT_ART: return "Realtime Art";
            case TEMPLATE::FILM: return "Film";
            case TEMPLATE::GAME: return "Game";
            case TEMPLATE::SOFTWARE: return "Software";
            case TEMPLATE::SERVER: return "Server";
        }
    }

    void JPopupProjectDashboard::GenerateProject(){
        fs::path r = p_path;
        r /= p_name;
        if(!fs::exists(r)) fs::create_directories(r);
        fs::path mani = r;
        mani /= MANIFEST_FILENAME;
        if(!fs::exists(mani)) {
            mINI::INIFile file(mani);
            mINI::INIStructure ini;

            ini["header"]["name"] = p_name.c_str();
            ini["header"]["description"] = p_text.c_str();
            ini["header"]["version"] = "0.0.1";

            file.generate(ini, true);
        }else{
            spdlog::error("Project path is already exist: {}", r.c_str());
            return;
        }
        jengine.context->project_path = r;
        jengine.context->load_project = true;
        SetEnable(false);
        spdlog::info("Project path successfully load: {}", r.c_str());
    }
}