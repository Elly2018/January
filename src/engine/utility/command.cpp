#include "command.h"
#include <vector>
#include <sstream>
#include <mutex>
#include <filesystem>
#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_notify.h"
#include "spdlog/spdlog.h"
#include "path.h"
#include "../../system/system.h"
#include "../../system/window.h"
#include "../../gui/manager.h"
#include "../../gui/popup/allpopup.h"
#include "../engine.h"
#include "../struct/config.h"
#include "../struct/context.h"

namespace fs = std::filesystem;

namespace January::Engine {
    void MyFolderCallback(void *userdata, const char *const *filelist, int filter) {
        System::JSystem* jsystem = (System::JSystem*)userdata;
        if (filelist == NULL || *filelist == NULL) {
            // User canceled the dialog or an error occurred
            SDL_Log("Dialog canceled or failed: %s", SDL_GetError());
        } else {
            // filelist[0] contains the path to the selected folder
            SDL_Log("Selected folder: %s", filelist[0]);
        }

        if (!(filelist == NULL || *filelist == NULL)) {
            std::string target_path = filelist[0];
            spdlog::info("Trying open path: {}", target_path);
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
            jsystem->engine->context->project_path = target_path;
            jsystem->engine->context->load_project = true;
            jsystem->engine->manager->project_dashboard->SetEnable(false);
            std::string t = std::format("Project path successfully load: {}", target_path.c_str());
            spdlog::info(t);
            ImGuiToast toast = ImGuiToast(ImGuiToastType_Success, 3000);
            toast.set_title("Project Load");
            toast.set_content(t.c_str());
            ImGui::InsertNotification(toast);
            AddRecent(*jsystem->engine, target_path);
        } else {
            std::string t = "Project path file dialog: cancel";
            spdlog::warn(t);
            ImGuiToast toast = ImGuiToast(ImGuiToastType_Warning, 3000);
            toast.set_title("Project Load");
            toast.set_content(t.c_str());
            ImGui::InsertNotification(toast);
            return;
        }
    }

    std::string GetPath(std::vector<std::string>& cmds){
        std::string r_path = "";
        for(int32_t i = 1; i < cmds.size(); i++){
            r_path += cmds.at(i);
            r_path += " ";
        }
        r_path.pop_back();
        return r_path;
    }

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
            std::string r_path = GetPath(cmds);
            bool exist = true;

            if(!fs::exists(r_path) || !fs::is_directory(r_path)) {
                std::string t = std::format("Project path does not exist: {}", r_path.c_str());
                spdlog::error(t);
                ImGuiToast toast = ImGuiToast(ImGuiToastType_Error, 3000);
                toast.set_title("Project Load");
                toast.set_content(t.c_str());
                ImGui::InsertNotification(toast);
                exist = false;
            }
            if(!is_project_path_vaild(r_path)){
                std::string t = std::format("Project path vaildation check failed: {}", r_path.c_str());
                spdlog::error(t);
                ImGuiToast toast = ImGuiToast(ImGuiToastType_Error, 3000);
                toast.set_title("Project Load");
                toast.set_content(t.c_str());
                ImGui::InsertNotification(toast);
                exist = false;
            }

            int32_t search = -1;
            for(int32_t i = 0; i < jsystem.engine->config->j_recent.size(); i++){
                if(jsystem.engine->config->j_recent.at(i).j_path == r_path){
                    search = i;
                    break;
                }
            }
            if(!exist) {
                RemoveRecent(*jsystem.engine, r_path);
                return;
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
        else if(cmds.size() >= 2 && cmds.at(0) == "create_folder"){
            std::string r_path = GetPath(cmds);
            jsystem.engine->manager->create_folder->RegisterFolder(r_path);
            jsystem.engine->manager->create_folder->SetEnable(true);
        }
        else if(cmds.size() >= 2 && cmds.at(0) == "create_resource"){
            std::string r_path = GetPath(cmds);
            jsystem.engine->manager->create_resource->RegisterFolder(r_path);
            jsystem.engine->manager->create_resource->SetEnable(true);
        }
        else if(cmds.size() >= 2 && cmds.at(0) == "create_script"){
            std::string r_path = GetPath(cmds);
            jsystem.engine->manager->create_script->RegisterFolder(r_path);
            jsystem.engine->manager->create_script->SetEnable(true);
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
            SDL_ShowOpenFolderDialog(MyFolderCallback, &jsystem, jsystem.window->g_window, NULL, false);            
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

