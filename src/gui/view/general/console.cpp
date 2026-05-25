#include "console.h"
#include <memory>
#include <imgui.h>
#include <imgui_stdlib.h>
#include "../../../engine/engine.h"
#include "../../../engine/struct/context.h"

namespace January::Engine::View {
    void JViewConsole::Init() {
        callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([&](const spdlog::details::log_msg &msg) {
            // for example you can be notified by sending an email to yourself
            ConsoleLog cl = ConsoleLog();
            cl.level = msg.level;
            cl.messages = std::string(msg.payload.data(), msg.payload.size());;
            cl.id = id_counter;
            changed = true;
            id_counter++;
            std::lock_guard<std::mutex> lock(log_mtx);
            auto_scroll_next = true;
            logs.push_back(cl);
        });
        callback_sink->set_level(spdlog::level::trace);
        jengine.context->logger = new spdlog::logger("engine logger", callback_sink);
        jengine.context->logger->info("Engine log created");
        spdlog::default_logger()->sinks().push_back(callback_sink);
    }

    void JViewConsole::DeInit(){
        Clear();
    }

    void JViewConsole::Draw() {
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
        if(changed){
            GetFilteredResult();
            changed = false;
        }
    }

    void JViewConsole::DrawBar(){
        if(ImGui::BeginCombo("Filter##console_view", GetName(level_filter).c_str())){
            for(int32_t n = 0; n < 6; n++){
                if(ImGui::Selectable( (GetName((spdlog::level::level_enum)n) + "##console_view_level_filter").c_str())){
                    level_filter = (spdlog::level::level_enum)n;
                    changed = true;
                }
            }
            ImGui::EndCombo();
        }
        if(ImGui::InputText("Search##console_view", &search)){
            changed = true;
        }
    }

    void JViewConsole::DrawContent(){
        std::lock_guard<std::mutex> lock(buffer_mtx);
        for(int32_t i = 0; i < buffer.size(); i++){
            std::string mesg = buffer.at(i).messages;
            bool content = mesg.starts_with("\t");
            if(content) continue;
            ImGui::PushStyleColor(ImGuiCol_Text, GetColor(buffer.at(i).level));
            bool is_selected = open_bottom == buffer.at(i).id;
            if(ImGui::Selectable((mesg + "##Console_Log_Index_" + std::to_string(i)).c_str(), is_selected)){
                open_bottom = buffer.at(i).id;
            }
            ImGui::PopStyleColor();
        }
    }

    void JViewConsole::DrawDetail() {
        std::lock_guard<std::mutex> lock(log_mtx);
        if (open_bottom < 0) return;
        if (open_bottom >= logs.size()) return;

        int32_t counter = open_bottom;
        int32_t line = 0;

        bool to_end = false;
        std::string mesg = logs.at(counter).messages;
        ImVec4 mesg_col = GetColor(logs.at(counter).level);
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
            ImGui::Text("%s", mesg.c_str());
            ImGui::PopStyleColor();
            line++;
            counter++;
            to_end = counter >= logs.size();

            if(!to_end){
                mesg = logs.at(counter).messages;
                mesg_col = GetColor(logs.at(counter).level);
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
        std::lock_guard<std::mutex> lock(buffer_mtx);
        std::lock_guard<std::mutex> lock2(log_mtx);
        buffer.clear();
        for(int32_t i = 0; i < logs.size(); i++){
            if(logs.at(i).level < level_filter){
                continue;
            }
            if(search.size() == 0 || logs.at(i).messages.find(search) != std::string::npos){
                buffer.push_back(logs.at(i));
            }
        }
    }

    void JViewConsole::Clear(){
        std::lock_guard<std::mutex> lock(buffer_mtx);
        buffer.clear();
        logs.clear();
        id_counter = 0;
    }
}