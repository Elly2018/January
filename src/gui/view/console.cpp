#include "console.h"
#include <imgui.h>
#include "../../engine/engine.h"
#include "../../engine/struct/context.h"

namespace January::Engine::View {
    void JViewConsole::Init() {
        callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([&](const spdlog::details::log_msg &msg) {
            // for example you can be notified by sending an email to yourself
            ConsoleLog cl;
            cl.level = msg.level;
            cl.messages = msg.payload.data();
            logs.push_back(cl);
        });
        callback_sink->set_level(spdlog::level::info);
        console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        jengine.context->logger = new spdlog::logger("engine logger", {console_sink, callback_sink});
    }

    void JViewConsole::Draw() {
        RenderBar();
        RenderContent();
    }

    void JViewConsole::Update() {
        
    }

    void JViewConsole::RenderBar(){

    }

    void JViewConsole::RenderContent(){
        for(int32_t i = 0; i < logs.size(); i++){
            ImGui::TextColored(GetColor(logs[i].level), "%s", logs[i].messages);
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
}