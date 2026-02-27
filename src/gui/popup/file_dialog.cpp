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
#include "../../engine/engine.h"
#include "../../engine/struct/context.h"
#include "../../engine/utility/path.h"

namespace January::Engine::View {
    std::string UnicodeToUTF8(uint32_t codepoint) {
        std::string result;
        if (codepoint <= 0x7F) {
            result += (char)codepoint;
        } else if (codepoint <= 0x7FF) {
            result += (char)(0xC0 | (codepoint >> 6));
            result += (char)(0x80 | (codepoint & 0x3F));
        } else if (codepoint <= 0xFFFF) {
            result += (char)(0xE0 | (codepoint >> 12));
            result += (char)(0x80 | ((codepoint >> 6) & 0x3F));
            result += (char)(0x80 | (codepoint & 0x3F));
        } else {
            result += (char)(0xF0 | (codepoint >> 18));
            result += (char)(0x80 | ((codepoint >> 12) & 0x3F));
            result += (char)(0x80 | ((codepoint >> 6) & 0x3F));
            result += (char)(0x80 | (codepoint & 0x3F));
        }
        return result;
    }

    void JPopupFileDialog::OnDisable(){
        JPopupBase::OnDisable();
        filters.clear();
        feedback = NULL;
    }

    void JPopupFileDialog::OnEnable(){
        path_dirty = get_home_directory().string();
        path = path_dirty;
        load = true;
        JPopupBase::OnEnable();
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
        ImGui::SetNextWindowPos(ImVec2(unit.x * 0.5F, unit.y * 0.5F), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(unit.x * 9, unit.y * 9), ImGuiCond_Always);
        return JPopupBase::PreDraw();
    }

    void JPopupFileDialog::Draw() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec2 winsize = ImGui::GetWindowSize();
        winsize.x -= (style.WindowPadding.x + style.FramePadding.x);
        winsize.y -= (style.WindowPadding.y + style.FramePadding.y);
        float bottom = ImGui::GetFrameHeightWithSpacing() * 3.5F;
        if(ImGui::BeginChild("URL Field##project_dashboard", ImVec2(winsize.x, ImGui::GetFrameHeightWithSpacing()), 0, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoDecoration)){
            DrawURLBar();
            ImGui::EndChild();
        }

        if(ImGui::BeginChild("Dirs##project_dashboard", ImVec2(((winsize.x / 5) * 1) - (style.ItemSpacing.x / 2), winsize.y - bottom), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoNav)){
            DrawDirRegion();
            ImGui::EndChild();
        }
        ImGui::SameLine();
        if(ImGui::BeginChild("Dir Contents##project_dashboard", ImVec2(((winsize.x / 5) * 4) - (style.ItemSpacing.x / 2), winsize.y - bottom), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoNav)){
            DrawContentRegion();
            ImGui::EndChild();
        }
        
        if(ImGui::Button("Confirm", ImVec2(winsize.x / 2 - (style.ItemSpacing.x / 2), ImGui::GetFrameHeightWithSpacing()))){
            SetEnable(false);
            spdlog::debug("File dialog confirm:");
            for(int32_t i = 0; i < selection.size(); i++){
                std::string buffer = selection.at(i);
                selection[i] = path + "/" + buffer;
                spdlog::debug("\t{}, path: {}", i, selection[i]);
            }
            if(feedback != NULL){
                feedback(true, selection);
            }
            selection.clear();
            feedback = NULL;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel", ImVec2(winsize.x / 2 - (style.ItemSpacing.x / 2), ImGui::GetFrameHeightWithSpacing()))){
            SetEnable(false);
            if(feedback != NULL){
                feedback(false, std::vector<std::string>());
            }
            selection.clear();
            feedback = NULL;
        }
    }

    void JPopupFileDialog::Update(){
        if(load){
            spdlog::debug("File Dialog load !");
            selection.clear();
            contents_dir.clear();
            contents_file.clear();
            std::thread t1 = std::thread([&](){
                LoadContent();
            });
            cid = t1.get_id();
            t1.detach();
            load = false;
        }
    }

    void JPopupFileDialog::DrawURLBar(){
        ImGui::PushFont(jengine.context->icon_font);
        {
            // Last page
            if(ImGui::Button((UnicodeToUTF8(0xF060) + "##File_Dialog_LastPage").c_str())){
                fs::path p = path;
                p = p.parent_path();
                path = p.string();
                path_dirty = path;
                load = true;
            }
            ImGui::SameLine();
        }
        ImGui::PopFont();


        if(ImGui::InputText("URL##project_dashboard", &path_dirty)){
            if(!fs::exists(path_dirty)) {
                path_dirty = path;
            } else {
                path = path_dirty;
                load = true;
            }
        }
        ImGui::SameLine();
        
        ImGui::PushFont(jengine.context->icon_font);
        {        
            // Last page
            if(ImGui::Button((UnicodeToUTF8(0xF009) + "##File_Dialog_Grid").c_str())){
                display_type = DisplayType::GRID;
            }
            ImGui::SameLine();
            // List of elements
            ImGui::SameLine();
            // List of elements
            if(ImGui::Button((UnicodeToUTF8(0xF03A) + "##File_Dialog_List").c_str())){
                display_type = DisplayType::LIST;
            }
            ImGui::SameLine();
            ImGui::Dummy(ImVec2(10, 0));
            ImGui::SameLine();
            // Add favorite 
            if(ImGui::Button(UnicodeToUTF8(0xF004).c_str())){
                std::lock_guard<std::mutex> lock(favorite_mtx);
                favorite.push_back(path.c_str());
            }
        }
        ImGui::PopFont();
    }

    void JPopupFileDialog::DrawDirRegion(){
        if(ImGui::Selectable("Home##project_dashboard_favorite_default")){
            path_dirty = get_home_directory().string();
            path = path_dirty;
            load = true;
        }
        if(favorite.size() > 0) ImGui::Separator();
        std::lock_guard<std::mutex> lock(favorite_mtx);
        for(auto& fav : favorite){
            std::string label = fs::path(fav).filename().string();
            int32_t flag = 0;
            if(!fs::exists(fav) || !fs::is_directory(fav)){
                flag = ImGuiSelectableFlags_Disabled;
            }
            if(ImGui::Selectable((label + "##project_dashboard_favorite").c_str(), false, flag)){
                path_dirty = fav;
                path = path_dirty;
                load = true;
            }
            ImGui::PushFont(jengine.context->icon_font);
            ImGui::SameLine();
            if(ImGui::Button(UnicodeToUTF8(0xF00D).c_str())){

            }
            ImGui::PopFont();
        }
    }

    void JPopupFileDialog::DrawContentRegion(){
        std::lock_guard<std::mutex> lock(content_mtx);
        for(auto& dir : contents_dir){
            bool isSelect = false;
            int32_t select_index = -1;
            for(int32_t i = 0; i < selection.size(); i++){
                if(selection.at(i) == dir){
                    isSelect = true;
                    select_index = i;
                    break;
                }
            }

            ImGui::PushFont(jengine.context->icon_font);
            ImGui::Text((UnicodeToUTF8(0xF07B)).c_str());
            ImGui::PopFont();
            ImGui::SameLine();
            if(ImGui::Selectable((dir + "##project_dashboard_dir_contents").c_str(), isSelect)){
                if(ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)){
                    if(isSelect){
                        selection.erase(selection.begin() + select_index);
                    }else{
                        if(dialog_type == DialogType::SINGLE_DIR){
                            selection.clear();
                            selection.push_back(dir);
                        }else{
                            selection.push_back(dir);
                        }
                    }
                }else{
                    selection.clear();
                    selection.push_back(dir);
                }
            }
            if(ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && ImGui::IsItemHovered()){
                spdlog::debug("Enter {} folder", dir);
                path_dirty += "/";
                path_dirty += dir;
                path += "/";
                path += dir;
                load = true;
            }
        }
        if(dialog_type != DialogType::SINGLE_DIR){
            for(auto& file : contents_file){
                bool pass = false;
                if(filters.size() != 0){
                    for(auto& f : filters){
                        if(file.ends_with(f.first)){
                            pass = true;
                            break;
                        }
                    }
                }else{
                    pass = true;
                }

                if(!pass) continue;

                bool isSelect = false;
                int32_t select_index = -1;
                for(int32_t i = 0; i < selection.size(); i++){
                    if(selection.at(i) == file){
                        isSelect = true;
                        select_index = i;
                        break;
                    }
                }
                
                ImGui::PushFont(jengine.context->icon_font);
                ImGui::Text(UnicodeToUTF8(0xF016).c_str());
                ImGui::PopFont();
                ImGui::SameLine();
                if(ImGui::Selectable((file + "##project_dashboard_dir_contents").c_str(), isSelect)){
                    if(ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)){
                        if(isSelect){
                            selection.erase(selection.begin() + select_index);
                        }else{
                            selection.push_back(file);
                        }
                    }else{
                        selection.clear();
                        selection.push_back(file);
                    }
                }
            }
        }
    }

    void JPopupFileDialog::LoadContent() {
        if(!fs::exists(path)) return;
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