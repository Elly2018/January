/**
MIT License

Copyright (c) 2026 ZhuElly

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "create_script.h"
#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <thread>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <spdlog/spdlog.h>
#include "script_template.h"
#include "../../../engine/assets/script_asset.h"

namespace fs = std::filesystem;

namespace January::Engine::View {
    void JPopupCreateScript::RegisterFolder(std::string _folder){
        folder = _folder;
    }

    void JPopupCreateScript::Init(){
        SetPopupSize(ImVec2(0.8f, 0.8f), true, true, false);
        spdlog::info("Loaded Popup: Create Script");
        window_flag |= ImGuiWindowFlags_NoScrollbar;
    }

    void JPopupCreateScript::Update() {
        if (!trigger_create.load(std::memory_order_relaxed)) {
            return; 
        }

        std::string buffer = "";
        {
            std::lock_guard<std::mutex> lock(input_mtx);
            buffer = input;
        }

        if(buffer.size() == 0){
            {
                std::lock_guard<std::mutex> lock(message_mtx);
                message = "";
            }
            can_be_confirm.store(false, std::memory_order_release);
            return;
        }

        if(fs::exists(folder)){
            fs::path f = folder;
            f /= buffer;
            if(fs::exists(f)){
                {
                    std::lock_guard<std::mutex> lock(message_mtx);
                    message = "Folder is already exist";
                }
                can_be_confirm.store(false, std::memory_order_release);
            }else{
                {
                    std::lock_guard<std::mutex> lock(message_mtx);
                    message = "";
                }
                can_be_confirm.store(true, std::memory_order_release);
            }
        }else{
            spdlog::error("Create folder failed, the base folder path does not exist: {}", folder);
            {
                std::lock_guard<std::mutex> lock(message_mtx);
                message = "Base folder is not exist, You can exit now";
            }
            can_be_confirm.store(false, std::memory_order_release);
        }

        trigger_create.store(false, std::memory_order_relaxed);
    }

    void JPopupCreateScript::Draw() {
        {
            std::lock_guard<std::mutex> lock(input_mtx);
            trigger_create.store(
                ImGui::InputText("Script Name##Create_Script_Field", &input), 
                std::memory_order_release);
        }
        size_t type_size = sizeof(JScriptAssetBase::ScriptTypeString) / sizeof(JScriptAssetBase::ScriptTypeString[0]);
        std::string type_text = JScriptAssetBase::ScriptTypeString[type.load()];
        if(ImGui::BeginCombo("Type", type_text.c_str())){
            for(int32_t i = 0; i < type_size; i++){
                std::string buffer = JScriptAssetBase::ScriptTypeString[i];
                if(ImGui::Selectable((buffer + "##Create_Script_Type_Option").c_str(), type == i)){
                    type.store(i, std::memory_order_release);
                }
            }
            ImGui::EndCombo();
        }
        ImGui::BeginDisabled(!can_be_confirm.load(std::memory_order_acquire));
        if(ImGui::Button("Confirm")){
            fs::path f = folder;
            f /= input;
            std::string ct = GetScriptTemplate(type.load());
            std::thread([f, ct]() {
                try{
                    std::fstream file(f.string());
                    
                    if(!file.is_open()){
                        spdlog::error("Failed to create script: {}", f.string());
                        return;
                    }

                    if (file.is_open()) {
                        file << ct;
                    }else{
                        spdlog::error("Script file output error !");
                    }
                    file << ct;
                    
                } catch (const std::exception& e) {
                    spdlog::error("Failed to create script background: {}", e.what());
                }
            }).detach();
            SetEnable(false);
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            SetEnable(false);
        }
        {
            std::lock_guard<std::mutex> lock(message_mtx);
            ImGui::TextColored(ImVec4(1.0, 0.2f, 0.2f, 1.0f), "%s", message.c_str());
        }
    }
}