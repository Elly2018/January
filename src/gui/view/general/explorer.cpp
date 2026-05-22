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
#include "explorer.h"
#include <filesystem>
#include <thread>
#include <spdlog/spdlog.h>
#include <imgui.h>
#include "../../../engine/engine.h"
#include "../../../engine/struct/config.h"
#include "../../../engine/struct/context.h"
#include "../../../engine/utility/format.h"

namespace fs = std::filesystem;

namespace January::Engine::View {
    void JViewExplorer::Init() {
        JViewBase::Init();
        spdlog::info("Loaded View: Explorer");
        Assets.name = "Assets";
    }

    void JViewExplorer::Update() {
        if(changed){
            spdlog::debug("Detect explorer update");
            fs::path pp = CurrentFolder();
            if(fs::exists(pp)){
                if(path != "/" && path != "") pp /= path.c_str();
                if(!fs::exists(pp)){
                    spdlog::warn("\tPath does not exist: {}, So we change back to project root instead", pp.c_str());
                    pp = fs::path(jengine.context->project_path.c_str());
                    path = CurrentFolder();
                }
                if(watcher != nullptr){
                    delete watcher;
                }
                spdlog::debug("\tRemove explorer file watcher");

                spdlog::debug("\tTry register watcher: {}", pp.c_str());
                watcher = new filewatch::FileWatch<std::string>(
                    pp.string(),
                    [this](const std::string& path, const filewatch::Event change_type){
                        changed = true;
                    }
                );
                spdlog::debug("\tCreate explorer file watcher");

                files.clear();
                spdlog::debug("\tClean file data");
                UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
                {
                    std::lock_guard<std::mutex> guard(mtx);
                    spdlog::debug("\tStart fetch files...");
                    for(auto entry : fs::directory_iterator(pp)){
                        spdlog::debug("\t\tDetect entry: {}", entry.path().c_str());
                        JFileContent file = JFileContent();
                        file.uuid = uuidGenerator.getUUID();
                        file.is_dir = entry.is_directory();
                        file.path = entry.path();
                        file.title = entry.path().filename();
                        if(!entry.is_directory()){
                            file.filesize = entry.file_size();
                        }
                        files.push_back(file);

                        spdlog::info("Assgin file watch event to {}", entry.path().string().c_str());
                    }
                }
            }else{
                spdlog::debug("\tSkip update because project path not exist");
            }
            changed = false;
        }
    }

    void JViewExplorer::Draw() {
        ImGuiStyle& style = ImGui::GetStyle();
        if(!init){
            changed = true;
            init = true;
        }
        float w = std::max(ImGui::GetWindowWidth(), 20.f);
        leftWidth = w * (1.f / 3.f);
        rightWidth = w * (2.f / 3.f);

        AppContext* context = jengine.context;
        if(context == nullptr){
            spdlog::error("context is nullptr");
        }

        if(context != nullptr && fs::exists(context->project_path)){
            bool change = ImGui::InputText("Path", path.data(), 512, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);
            if(change) changed = true;
            ImGui::SliderFloat("size", &imgSize, 0.f, 1.f, "%.1f");
            ImGui::BeginChild("ViewExplorer_Left", ImVec2(leftWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true);
                DrawLeftSide();
            ImGui::EndChild();
            ImGui::SameLine();
            ImGui::BeginChild("ViewExplorer_Right", ImVec2(rightWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true);
                DrawRightSide();
            ImGui::EndChild();
            DrawRightSide_Event();
        }else{
            ImGui::Text("You need to choose path fist");
        }
    }

    void JViewExplorer::DeInit() {
        JViewBase::DeInit();
    }

    void JViewExplorer::Focus(bool value) {
        JViewBase::Focus(value);
        changed = true;
    }

    void JViewExplorer::ReloadProject(){
        path = "";
        init = false;
    }

    void JViewExplorer::DrawLeftSide(){
        ImGui::Text("Left");
    }

    void JViewExplorer::DrawRightSide(){
        if(imgSize == 0){
            ImGui::BeginListBox("Explorer_Right_Listbox", ImVec2(rightWidth, 0));
            int32_t c = 0;
            for(auto file : files){
                ImGui::Selectable(file.title.c_str(), c == selection);
                c++;
            }
            ImGui::EndListBox();
        }else{
            ImGui::BeginGroup();
            int32_t c = 0;
            int32_t row = std::floor<int32_t>(rightWidth / std::max<float>(imgSize * 500, 50));
            for(auto file : files){
                ImGui::Button(file.title.c_str(), ImVec2(std::max<float>(imgSize * 500, 50), std::max<float>(imgSize * 500, 50)));
                DrawItemTooltip(file);
                if((c + 1) % row != 0){
                    ImGui::SameLine();
                }
                c++;
            }
            ImGui::EndGroup();
        }
    }

    void JViewExplorer::DrawItemTooltip(JFileContent& target){
        std::string display_text = "";
        display_text += "Filename: ";
        display_text += target.path.filename().string().c_str();
        display_text += "\n";

        display_text += "Folder: ";
        display_text += target.path.parent_path().string().c_str();
        display_text += "\n";

        display_text += "Size: ";
        display_text += format_bytes(target.filesize);
        display_text += "\n";

        ImGui::SetItemTooltip("%s", display_text.c_str());
    }

    void JViewExplorer::DrawRightSide_Event(){
        if(ImGui::BeginPopupContextItem("ViewExplorer_Right_ContextItem")){
            if (ImGui::Selectable("Create Folder")){
                
            }
            if (ImGui::Selectable("Create Resource")){
                
            }
            if (ImGui::Selectable("Create Script")){
                
            }
            if (ImGui::Selectable("Refresh")){
                
            }
            ImGui::EndPopup();
        }
    }

    fs::path JViewExplorer::CurrentFolder(){
        fs::path p = jengine.context->project_path;
        p /= "Assets";
        p /= path;
        return p;
    }
}