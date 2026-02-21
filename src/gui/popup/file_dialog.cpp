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
#include "file_dialog.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include <spdlog/spdlog.h>
#include "../../engine/utility/path.h"

namespace January::Engine::View {
    void JPopupFileDialog::OnDisable(){
        JPopupBase::OnDisable();
        filters.clear();
        feedback = NULL;
    }

    void JPopupFileDialog::SetTitle(std::string _newtitle){
        title = _newtitle + "##popup_file_dialog";
    }

    void JPopupFileDialog::SetFilter(std::vector<std::pair<std::string, std::string>> _filters){
        filters.clear();
        filters = _filters;
    }

    void JPopupFileDialog::SetDialogType(DialogType _dialog_type){
        dialog_type = _dialog_type;
    }

    void JPopupFileDialog::RegisterOneTimeFeedback(DialogResultFeedback _feedback){
        feedback = _feedback;
    }

    void JPopupFileDialog::Init() {
        path_dirty = get_home_directory().string();
        path = path_dirty;
    }

    bool JPopupFileDialog::PreDraw() {
        window_flag = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

        ImGuiIO& io = ImGui::GetIO();
        ImVec2 display = io.DisplaySize;
        ImVec2 unit = ImVec2(display.x / 10.0f, display.y / 10.0f);
        ImGui::SetNextWindowPos(ImVec2(unit.x * 1, unit.y * 1), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(unit.x * 8, unit.y * 8), ImGuiCond_Always);
        return JPopupBase::PreDraw();
    }

    void JPopupFileDialog::Draw() {
        ImVec2 winsize = ImGui::GetWindowSize();
        if(ImGui::BeginChild("URL Field##project_dashboard", ImVec2(winsize.x, 80), 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration)){
            if(ImGui::InputText("URL##project_dashboard", &path_dirty)){
                if(!fs::exists(path_dirty)) {
                    path_dirty = path;
                } else {
                    path = path_dirty;
                    contents_dir.clear();
                    contents_file.clear();
                    load = true;
                }
            }
            ImGui::EndChild();
        }

        if(ImGui::BeginChild("Dirs##project_dashboard", ImVec2( (winsize.x / 5) * 1, winsize.y - 250), 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration)){
            ImGui::Selectable("TEST##project_dashboard_dirs");
            ImGui::EndChild();
        }
        ImGui::SameLine();
        if(ImGui::BeginChild("Dir Contents##project_dashboard", ImVec2( (winsize.y / 5) * 1, winsize.y - 250), 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration)){
            {
                std::lock_guard<std::mutex> lock(content_mtx);
                for(auto& dir : contents_dir){
                    ImGui::Selectable((dir + "##project_dashboard_dir_contents").c_str());
                }
                for(auto& file : contents_file){
                    ImGui::Selectable((file + "##project_dashboard_dir_contents").c_str());
                }
            }
            ImGui::EndChild();
        }

        if(ImGui::Button("Confirm", ImVec2(winsize.x / 2, 0))){
            
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(winsize.x / 2, 0))){
            SetEnable(false);
        }
    }

    void JPopupFileDialog::Update(){
        if(load){
            spdlog::debug("File Dialog load !");
            std::thread t1 = std::thread([&](){
                LoadContent();
            });
            cid = t1.get_id();
            t1.detach();
            load = false;
        }
    }

    void JPopupFileDialog::LoadContent() {
        if(!fs::is_directory(path)) return;
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        std::string this_thread_id = oss.str();

        int32_t times = 0;
        while(cid != std::this_thread::get_id()) {
            times++;
            std::this_thread::sleep_for(std::chrono::microseconds(50));
            if(times > 20) return;
        }
        spdlog::debug("File Dialog starting detecting dir contents !");
        for(const auto& entry : fs::directory_iterator(path)){
            if(cid != std::this_thread::get_id()) {
                spdlog::debug("File Dialog load content getting cancel: {}", this_thread_id);
                break;
            }
            std::lock_guard<std::mutex> lock(content_mtx);
            std::string buffer = entry.path().filename();
            if(entry.is_directory()){
                contents_dir.push_back(buffer);
                spdlog::debug("\tFile Dialog add dir: {}", buffer);
            }else{
                contents_file.push_back(buffer);
                spdlog::debug("\tFile Dialog add file: {}", buffer);
            }
        }
        spdlog::debug("File Dialog load content finish loading: {}", this_thread_id);
    }
}