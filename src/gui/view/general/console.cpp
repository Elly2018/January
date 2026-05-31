#include "console.h"
#include <functional>
#include <memory>
#include <imgui.h>
#include <imgui_stdlib.h>
#include "../../../engine/engine.h"
#include "../../../engine/struct/context.h"
#include "../../../engine/utility/logger.h"

namespace January::Engine::View {
    void JViewConsole::Init() {
        spdlog::info("Engine log created");
        spdlog::info("\tSubline Testing");
        window_flag |= ImGuiWindowFlags_MenuBar;
    }

    void JViewConsole::DeInit(){
        Clear();
    }

    void JViewConsole::Draw() {
        DrawMenu();
        DrawBar();
        ImGui::Separator();

        ImGuiStyle& style = ImGui::GetStyle();
        float h = ImGui::GetContentRegionAvail().y;
        if(!init){
            init = true;
            topHeight = 200.0f;
            spdlog::debug("ConsoleLog init");
            spdlog::debug("\th init value: {}", h);
            spdlog::debug("\ttop height init value: {}", topHeight.load());
        }

        DrawTab();
        DrawTabContent();
    }

    void JViewConsole::DrawMiddleHandle(float total_window_height, float splitterHeight){
        const float minHeight = 50.0f;
        const float maxHeight = total_window_height - 50.0f;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::InvisibleButton("h_splitter##Console_Splitter", ImVec2(-1, splitterHeight));

        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
        }

        static bool active = false;

        if (ImGui::IsItemActive()) {
            float delta = ImGui::GetIO().MouseDelta.y;
            if (delta != 0.0f) {
                float og = topHeight;
                float newTopHeight = topHeight + delta;
                if (newTopHeight < minHeight) newTopHeight = minHeight;
                if (newTopHeight > maxHeight) newTopHeight = maxHeight;
                
                if(!active){
                    spdlog::trace("ConsoleLog splitter interact data");
                    spdlog::trace("\tInteract Part");
                    spdlog::trace("\t\tOriginal value: {}", og);
                    spdlog::trace("\t\tNew value: {}", newTopHeight);
                    spdlog::trace("\t\tDelta: {}", delta);
                    spdlog::trace("\tConst Part");
                    spdlog::trace("\t\tTotal window height: {}", total_window_height);
                    spdlog::trace("\t\tMinimum height: {}", minHeight);
                    spdlog::trace("\t\tMaximum height: {}", maxHeight);
                    active = true;
                }
                topHeight = newTopHeight;
            }
        }else{
            active = false;
        }

        // 4. Right Panel
        ImGui::PopStyleVar();
    }

    void JViewConsole::Update() {
        if(jengine.context->logger->logger->IsChanged() || change_page.load() || change_content.load()){
            if(change_content.load()){
                spdlog::trace("Console view: tchange_page !");
                open_bottom = -1;
                change_content.store(false);
            }
            GetFilteredResult();
            change_page.store(false);
        }
    }

    void JViewConsole::DrawMenu(){
        if(ImGui::BeginMenuBar()){
            if(ImGui::BeginMenu("Debug##Console_Log_View_Menu_Debug")){
                if(ImGui::MenuItem("MSG_COUNT", NULL, show_line_count.load())){
                    show_line_count.store(!show_line_count.load());
                }
                if(ImGui::MenuItem("MSG_ID", NULL, show_id.load())){
                    show_id.store(!show_id.load());
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void JViewConsole::DrawBar(){
        if(ImGui::BeginCombo("Filter##console_view", GetName(level_filter).c_str())){
            for(int32_t n = 0; n < 6; n++){
                if(ImGui::Selectable( (GetName((spdlog::level::level_enum)n) + "##console_view_level_filter").c_str())){
                    level_filter = (spdlog::level::level_enum)n;
                    change_page.store(true);
                }
            }
            ImGui::EndCombo();
        }
        if(ImGui::InputText("Search##console_view", &search)){
            change_page.store(true);
        }
    }

    void JViewConsole::DrawTab(){
        if(ImGui::BeginTabBar("ViewConsole_Tag")){
            if(ImGui::BeginTabItem("Engine")){
                if(logger_index != 0){
                    logger_index = 0;
                    change_page.store(true);
                    change_content.store(true);
                }
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("Runtime")){
                if(logger_index != 1){
                    logger_index = 1;
                    change_page.store(true);
                    change_content.store(true);
                }
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("Script")){
                if(logger_index != 2){
                    logger_index = 2;
                    change_page.store(true);
                    change_content.store(true);
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }

    void JViewConsole::DrawTabContent(){
        ImGuiStyle& style = ImGui::GetStyle();
        float h = ImGui::GetContentRegionAvail().y;
        if(open_bottom >= 0){
            {
                ImGui::BeginChild("ViewConsole_Frame", ImVec2(0, 0), false, ImGuiWindowFlags_NoSavedSettings);
                float current_top_h = topHeight;
                if (current_top_h > h - 100.0f) current_top_h = h - 100.0f;
                if (current_top_h < 50.0f) current_top_h = 50.0f;
                {
                    ImGui::BeginChild("ViewConsole_Top", ImVec2(0, (topHeight - (style.DisplayWindowPadding.y / 1.5f))), true, ImGuiWindowFlags_NoSavedSettings);
                    DrawContent();
                    ImGui::EndChild();
                }
                DrawMiddleHandle(h, 8);
                {
                    ImGui::BeginChild("ViewConsole_Bottom", ImVec2(0, 0), true, ImGuiWindowFlags_NoSavedSettings);
                    DrawDetail();
                    ImGui::EndChild();
                }
                ImGui::EndChild();
            }
        }else{
            {
                ImGui::BeginChild("ViewConsole_Top", ImVec2(0, 0), true, ImGuiWindowFlags_NoSavedSettings);
                DrawContent();
                ImGui::EndChild();
            }
        }
    }

    void JViewConsole::DrawContent(){
        std::lock_guard<std::mutex> lock(buffer_mtx);
        std::string last_line = "";
        int32_t last_index = -1;
        uint32_t counter = 1;

        std::function<void(uint32_t)> submit = std::function<void(uint32_t)>([&](uint32_t i) {
            ImGui::PushStyleColor(ImGuiCol_Text, GetColor(buffer.at(i).level));
            bool is_selected = open_bottom == buffer.at(i).id;
            std::string form = "";
            if(show_line_count.load() && show_id.load()){
                form = std::format("[{}|{}] {}##Console_Log_Index_{}", counter, buffer.at(i).id, last_line.c_str(), i);
            }
            else if(!show_line_count.load() && show_id.load()){
                form = std::format("[{}] {}##Console_Log_Index_{}", buffer.at(i).id, last_line.c_str(), i);
            }
            else if(show_line_count.load() && !show_id.load()){
                form = std::format("[{}] {}##Console_Log_Index_{}", counter, last_line.c_str(), i);
            }
            else {
                form = std::format("{}##Console_Log_Index_{}", last_line.c_str(), i);
            }
            
            if(ImGui::Selectable(form.c_str(), is_selected)){
                open_bottom = buffer.at(i).id;
            }
            ImGui::PopStyleColor();
        }); 

        for(int32_t i = 0; i < buffer.size(); i++){
            std::string mesg = buffer.at(i).messages;

            if(mesg.starts_with("\t")) {
                counter++;
                continue;
            }
            if(last_index != i && last_index != -1){
                submit(last_index);
                counter = 1;
            }
            last_index = i;
            last_line = mesg;
        }

        if (last_index != -1) {
            submit(last_index);
        }
    }

    void JViewConsole::DrawDetail() {
        JLoggerWorker* instance = GetLogger();
        if(instance == nullptr) return;
        std::lock_guard<std::mutex> lock(instance->log_mtx);
        if (open_bottom < 0) return;
        if (open_bottom >= instance->logs.size()) return;

        int32_t counter = open_bottom;
        int32_t line = 0;

        bool to_end = false;
        std::string mesg = instance->logs.at(counter).messages;
        uint32_t mesg_index = instance->logs.at(counter).id;
        ImVec4 mesg_col = GetColor(instance->logs.at(counter).level);
        do {
            int32_t ident = 0;
            while(mesg.starts_with("\t")){
                ident++;
                mesg.erase(mesg.begin());
            }
            for(int32_t j = 0; j < ident; j++) {
                ImGui::Indent(25.0f);
            }

            ImGui::PushStyleColor(ImGuiCol_Text, mesg_col);
            if(show_id.load()){
                ImGui::Text("[%i] %s", mesg_index, mesg.c_str());
            }else{
                ImGui::Text("%s", mesg.c_str());
            }
            ImGui::PopStyleColor();
            line++;
            counter++;
            to_end = counter >= instance->logs.size();

            if(!to_end){
                mesg = instance->logs.at(counter).messages;
                mesg_index = instance->logs.at(counter).id;
                mesg_col = GetColor(instance->logs.at(counter).level);
            }

            for(int32_t j = 0; j < ident; j++) {
                ImGui::Unindent(25.0f);
            }
        } while (mesg.starts_with("\t") && !to_end);
    }

    const struct ImVec4 JViewConsole::GetColor(spdlog::level::level_enum col){
        switch(col){
            case spdlog::level::level_enum::trace: return ImVec4(0.4F, 0.6F, 0.4F, 1.0F);
            case spdlog::level::level_enum::debug: return ImVec4(0.6F, 0.6F, 0.6F, 1.0F);
            default:
            case spdlog::level::level_enum::info: return ImVec4(1.0F, 1.0F, 1.0F, 1.0F);
            case spdlog::level::level_enum::warn: return ImVec4(1.0F, 1.0F, 0.0F, 1.0F);
            case spdlog::level::level_enum::err: return ImVec4(1.0F, 0.0F, 0.0F, 1.0F);
            case spdlog::level::level_enum::critical: return ImVec4(1.0F, 0.0F, 0.0F, 1.0F);
            case spdlog::level::level_enum::off: return ImVec4(0, 0, 0, 0);
            case spdlog::level::level_enum::n_levels: return ImVec4(0, 0, 0, 0);
        };
    }

    std::string JViewConsole::GetName(spdlog::level::level_enum col){
        switch(col){
            case spdlog::level::level_enum::trace: return "Trace";
            case spdlog::level::level_enum::debug: return "Debug";
            default:
            case spdlog::level::level_enum::info: return "Info";
            case spdlog::level::level_enum::warn: return "Warn";
            case spdlog::level::level_enum::err: return "Error";
            case spdlog::level::level_enum::critical: return "Critical";
            case spdlog::level::level_enum::off: return "Off";
            case spdlog::level::level_enum::n_levels: return "n_level";
        };
    }

    void JViewConsole::GetFilteredResult(){
        JLoggerWorker* instance = GetLogger();
        if(instance == nullptr) return;
        std::lock_guard<std::mutex> lock(buffer_mtx);
        std::lock_guard<std::mutex> lock2(instance->log_mtx);
        buffer.clear();
        for(int32_t i = 0; i < instance->logs.size(); i++){
            if(instance->logs.at(i).level < level_filter){
                continue;
            }
            if(search.size() == 0 || instance->logs.at(i).messages.find(search) != std::string::npos){
                buffer.push_back(instance->logs.at(i));
            }
        }
    }

    void JViewConsole::Clear(){
        JLoggerWorker* instance = GetLogger();
        std::lock_guard<std::mutex> lock(buffer_mtx);
        buffer.clear();
        if(instance != nullptr){
            instance->Clear();
        }
    }

    JLoggerWorker* JViewConsole::GetLogger() {
        if(logger_index == -1 || jengine.context == nullptr || jengine.context->logger == nullptr) return nullptr;
        switch(logger_index)
        {
            default:
            case 0: return jengine.context->logger->logger;
            case 1: return jengine.context->logger->runtime_logger;
            case 2: return jengine.context->logger->script_logger;
        }
    }
}