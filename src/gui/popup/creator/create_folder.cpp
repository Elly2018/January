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
#include "create_folder.h"
#include <filesystem>
#include <thread>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace January::Engine::View {
    void JPopupCreateFolder::RegisterFolder(std::string _folder){
        folder = _folder;
    }

    void JPopupCreateFolder::Init(){
        SetPopupSize(ImVec2(800, 250), false, false);
        spdlog::info("Loaded Popup: Create Folder");
    }

    void JPopupCreateFolder::Update() {
        if (!trigger_create.load(std::memory_order_relaxed)) {
            return; 
        }

        std::string buffer = "";
        {
            std::lock_guard<std::mutex> lock(input_mtx);
            buffer = input;
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

    void JPopupCreateFolder::Draw(){
        {
            std::lock_guard<std::mutex> lock(input_mtx);
            trigger_create.store(
                ImGui::InputText("Folder Name##Create_Folder_Field", &input), 
                std::memory_order_release);
        }
        ImGui::BeginDisabled(!can_be_confirm.load(std::memory_order_acquire));
        if(ImGui::Button("Confirm")){
            fs::path f = folder;
            f /= input;
            std::thread([f]() {
                try{
                    fs::create_directories(f);
                } catch (const std::exception& e) {
                    spdlog::error("Failed to create directory background: {}", e.what());
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
            ImGui::TextColored(ImVec4(255, 100, 100, 255), "%s", message.c_str());
        }
    }
}