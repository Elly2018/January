#include "command.h"
#include <vector>
#include <sstream>
#include <mutex>
#include <filesystem>
#include "imgui.h"
#include "imgui_notify.h"
#include "spdlog/spdlog.h"
#include "path.h"
#include "../../system/system.h"
#include "../../system/window.h"
#include "../../gui/manager.h"
#include "../../gui/popup/file_dialog.h"
#include "../../gui/popup/project_dashboard.h"
#include "../engine.h"
#include "../struct/config.h"
#include "../struct/context.h"

namespace fs = std::filesystem;

namespace January::Engine {
    std::vector<std::string> split_string_by_space(const std::string& str) {
        std::vector<std::string> words;
        std::stringstream ss(str); // Turn the string into a stream
        std::string word;

        // The extraction operator (>>) automatically uses whitespace as a delimiter
        while (ss >> word) {
            words.push_back(word);
        }

        return words;
    }

    void multi_command(struct System::JSystem& jsystem, std::vector<std::string> cmds){
        if(cmds.size() >= 2 && cmds.at(0) == "open_recent"){
            std::string r_path = "";
            for(int32_t i = 1; i < cmds.size(); i++){
                r_path += cmds.at(i);
                r_path += " ";
            }
            r_path.pop_back();
            int32_t search = -1;
            for(int32_t i = 0; i < jsystem.engine->config->j_recent.size(); i++){
                if(jsystem.engine->config->j_recent.at(i).j_path == r_path){
                    search = i;
                    break;
                }
            }
            if(search != -1){
                jsystem.engine->config->j_recent[search].j_last_open = sc::system_clock::to_time_t(sc::system_clock::now());
            }else{
                AppConfigRecent acr;
                acr.j_path = r_path;
                acr.j_last_open = sc::system_clock::to_time_t(sc::system_clock::now());
                jsystem.engine->config->j_recent.push_back(acr);
            }
            SaveEnableConfig(*jsystem.engine->manager, *jsystem.engine->config);
            jsystem.engine->context->project_path = r_path;
            jsystem.engine->context->load_project = true;
            std::string t = std::format("Project path successfully load: {}", r_path.c_str());
            spdlog::info(t);
            ImGuiToast toast = ImGuiToast(ImGuiToastType_Success, 3000);
            toast.set_title("Project Load");
            toast.set_content(t.c_str());
            ImGui::InsertNotification(toast);
            AddRecent(*jsystem.engine, r_path);
        }
    }

    void single_command(struct System::JSystem& jsystem, std::string cmd){
        if(cmd == "config_dirty"){
            SaveEnableConfig(*jsystem.engine->manager, *jsystem.engine->config);
            SaveAppConfig(*jsystem.engine->config);
            System::SavePreference();
        }
        else if(cmd == "new_project"){
            jsystem.engine->manager->project_dashboard->SetEnable(true);
        }
        else if(cmd == "open_project"){
            View::DialogResultFeedback feedback = [&jsystem](bool cancel, std::vector<std::string> results) {
                if(cancel) {
                    std::string t = "Project path file dialog: cancel";
                    spdlog::warn(t);
                    ImGuiToast toast = ImGuiToast(ImGuiToastType_Warning, 3000);
                    toast.set_title("Project Load");
                    toast.set_content(t.c_str());
                    ImGui::InsertNotification(toast);
                    return;
                }
                if(results.size() < 1) {
                    std::string t = "Project path file dialog: no path select";
                    spdlog::warn(t);
                    ImGuiToast toast = ImGuiToast(ImGuiToastType_Warning, 3000);
                    toast.set_title("Project Load");
                    toast.set_content(t.c_str());
                    ImGui::InsertNotification(toast);
                    return;
                }
                std::string target_path = results.at(0);
                if(!fs::exists(target_path) || !fs::is_directory(target_path)) {
                    std::string t = std::format("Project path does not exist: {}", target_path.c_str());
                    spdlog::error(t);
                    ImGuiToast toast = ImGuiToast(ImGuiToastType_Error, 3000);
                    toast.set_title("Project Load");
                    toast.set_content(t.c_str());
                    ImGui::InsertNotification(toast);
                    return;
                }
                if(!is_project_path_vaild(target_path)){
                    std::string t = std::format("Project path vaildation check failed: {}", target_path.c_str());
                    spdlog::error(t);
                    ImGuiToast toast = ImGuiToast(ImGuiToastType_Error, 3000);
                    toast.set_title("Project Load");
                    toast.set_content(t.c_str());
                    ImGui::InsertNotification(toast);
                    return;
                }
                jsystem.engine->context->project_path = target_path;
                jsystem.engine->context->load_project = true;
                jsystem.engine->manager->project_dashboard->SetEnable(false);
                std::string t = std::format("Project path successfully load: {}", target_path.c_str());
                spdlog::info(t);
                ImGuiToast toast = ImGuiToast(ImGuiToastType_Success, 3000);
                toast.set_title("Project Load");
                toast.set_content(t.c_str());
                ImGui::InsertNotification(toast);
                AddRecent(*jsystem.engine, target_path);
            };
            std::vector<std::pair<std::string, std::string>> filters = std::vector<std::pair<std::string, std::string>>();
            filters.push_back(std::pair<std::string, std::string>(".january", "January Project"));
            jsystem.engine->manager->file_dialog->SetTitle("Open Project");
            jsystem.engine->manager->file_dialog->SetDialogType(View::JPopupFileDialog::DialogType::SINGLE_DIR);
            jsystem.engine->manager->file_dialog->SetFilter(filters);
            jsystem.engine->manager->file_dialog->SetEnable(true);
            jsystem.engine->manager->file_dialog->RegisterOneTimeFeedback(feedback);
        }
        else if(cmd == "save_project"){

        }
        else if(cmd == "save_project_as"){

        }
        else if(cmd == "open_setting"){

        }
        else if(cmd == "open_preference"){

        }
    }

    void ApplyCommand(struct System::JSystem& jsystem, std::string cmd){
        std::vector<std::string> cs = split_string_by_space(cmd);
        if(cs.size() == 1){
            single_command(jsystem, cmd);
        }else{
            multi_command(jsystem, cs);
        }
        spdlog::debug("Apply command: {}", cmd);
    }

    void PushCommand(struct AppContext& ctx, std::string cmd){
        std::lock_guard<std::mutex> lock(ctx.commands_mtx);
        ctx.commands.push(cmd);
        spdlog::debug("Push command: {}", cmd);
    }
}

