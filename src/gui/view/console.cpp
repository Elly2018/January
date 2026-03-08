#include "console.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include "../../engine/engine.h"
#include "../../engine/struct/context.h"

namespace January::Engine::View {
    void JViewConsole::Init() {
        callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([&](const spdlog::details::log_msg &msg) {
            // for example you can be notified by sending an email to yourself
            ConsoleLog cl = ConsoleLog();
            cl.level = msg.level;
            cl.messages = msg.payload.data();
            logs.push_back(cl);
            changed = true;
        });
        callback_sink->set_level(spdlog::level::info);
        console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        jengine.context->logger = new spdlog::logger("engine logger", {console_sink, callback_sink});
    }

    void JViewConsole::DeInit(){
        Clear();
    }

    void JViewConsole::Draw() {
        RenderBar();
        RenderContent();
    }

    void JViewConsole::Update() {
        if(changed){
            GetFilteredResult();
            changed = false;
        }
    }

    void JViewConsole::RenderBar(){
        if(ImGui::BeginCombo("Filter##console_view", GetName(level_filter).c_str())){
            for(int32_t n = 0; n < 6; n++){
                if(ImGui::Selectable( (GetName((spdlog::level::level_enum)n) + "##console_view_level_filter").c_str())){
                    level_filter = (spdlog::level::level_enum)n;
                    changed = true;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if(ImGui::InputText("Search##console_view", &search)){
            changed = true;
        }
    }

    void JViewConsole::RenderContent(){
        std::lock_guard<std::mutex> lock(buffer_mtx);
        for(int32_t i = 0; i < buffer.size(); i++){
            ImGui::TextColored(GetColor(buffer.at(i).level), "%s", buffer.at(i).messages.c_str());
        }
    }

    const struct ImVec4 JViewConsole::GetColor(spdlog::level::level_enum col){
        switch(col){
            case spdlog::level::level_enum::trace: return ImVec4(0.6F, 0.6F, 0.6F, 1.0F);
            case spdlog::level::level_enum::debug: return ImVec4(0.8F, 0.8F, 0.8F, 1.0F);
            default:
            case spdlog::level::level_enum::info: return ImVec4(1.0F, 1.0F, 1.0F, 1.0F);
            case spdlog::level::level_enum::warn: return ImVec4(1.0F, 1.0F, 0.0F, 1.0F);
            case spdlog::level::level_enum::err: return ImVec4(1.0F, 0.0F, 0.0F, 1.0F);
            case spdlog::level::level_enum::critical: return ImVec4(1.0F, 0.0F, 0.0F, 1.0F);
            case spdlog::level::level_enum::off: return ImVec4(0, 0, 0, 0);
            case spdlog::level::level_enum::n_levels: return ImVec4(0, 0, 0, 0);
        };
    }

    std::string GetName(spdlog::level::level_enum col){
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
        buffer.clear();
        for(int32_t i = 0; i < logs.size(); i++){
            if(logs.at(i).level < level_filter){
                continue;
            }
            if(search.size() == 0 || logs.at(i).messages.find(search.c_str())){
                buffer.push_back(logs.at(i));
            }
        }
    }

    void JViewConsole::Clear(){
        std::lock_guard<std::mutex> lock(buffer_mtx);
        buffer.clear();
        logs.clear();
    }
}