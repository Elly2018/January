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
#include <algorithm>
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

    void JFolderContent::CleanChildren(){
        for(auto c : children){
            c->CleanChildren();
            delete c;
        }
        children.clear();
    }

    void JViewExplorer::Init() {
        JViewBase::Init();
        spdlog::info("Loaded View: Explorer");
        Assets.name = "Assets";
        path = "Assets";
        path_node.clear();
        path_node.push_back("Assets");
    }

    void JViewExplorer::Update() {
        if(changed){
            spdlog::debug("Detect explorer update");
            fs::path pp = CurrentFolder();
            if(fs::exists(pp)){
                if(watcher != nullptr){
                    delete watcher;
                }
                spdlog::debug("\tRemove explorer file watcher");

                spdlog::debug("\tTry register watcher: {}", pp.c_str());
                spdlog::debug("\tCurrent path: {}", path);
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
                    std::lock_guard<std::mutex> guard(files_mtx);
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
                spdlog::debug("\tSkip update because project path not exist {}", pp.string());
            }
            changed = false;
        }
    }

    void JViewExplorer::Draw() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiIO& io = ImGui::GetIO();
        float w = std::max(ImGui::GetWindowWidth(), 20.f);
        if(!init){
            changed = true;
            init = true;
            leftWidth = w * (1.f / 3.f);
        }
        AppContext* context = jengine.context;
        if(context == nullptr){
            spdlog::error("context is nullptr");
        }

        if(context != nullptr && fs::exists(context->project_path)){
            {
                ImGui::BeginChild("ViewExplorer_Top", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY);
                DrawPathAction();
                ImGui::SameLine();
                DrawPathBar();
                ImGui::EndChild();
            }
            int32_t temp_imgSize = imgSize.load();
            ImGui::SliderInt("size", &temp_imgSize, 0, 10, "%d");
            {
                imgSize.store(temp_imgSize);
                ImGui::BeginChild("ViewExplorer_Left", ImVec2(leftWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true);
                DrawLeftSide();
                ImGui::EndChild();
            }
            DrawMiddleHandle();
            {
                ImGui::BeginChild("ViewExplorer_Right", ImVec2(0, 0), true);
                DrawRightSide();
                DrawRightSide_Event();
                ImGui::EndChild();
            }
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

    void JViewExplorer::DrawPathAction() {
        ImGui::PushFont(jengine.context->icon_font);
        ImGui::BeginDisabled(travel_record.size() == 0);
        if(ImGui::Button("\uf053")){ // Return
            std::lock_guard<std::mutex> guard(travel_record_mtx);
            changed = true;
            std::string buffer = travel_record.top();
            travel_record.pop();
            path = buffer;
            UpdatePathNode();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::BeginDisabled(path_node.size() == 1);
        if(ImGui::Button("\uf062")){ // Up
            std::lock_guard<std::mutex> guard(travel_record_mtx);
            changed = true;
            fs::path buffer = path;
            buffer = buffer.parent_path();
            travel_record.push(path);
            path = buffer.string();
            spdlog::debug("Asset browse {}", path);
            UpdatePathNode();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::BeginDisabled(path == "Assets");
        if(ImGui::Button("\uf015")){ // Home
            std::lock_guard<std::mutex> guard(travel_record_mtx);
            changed = true;
            travel_record.push(path);
            path = "Assets";
            spdlog::debug("Asset browse {}", path);
            UpdatePathNode();
        }
        ImGui::EndDisabled();
        ImGui::PopFont();
    }

    void JViewExplorer::DrawPathBar() {
        fs::path buffer = "";
        for(auto& p : path_node){
            buffer /= p;
            if(ImGui::Button((p + "##Explorer_Path_Button").c_str())){
                std::lock_guard<std::mutex> guard(travel_record_mtx);
                changed = true;
                travel_record.push(path);
                path = buffer;
                spdlog::debug("Asset browse {}", path);
                UpdatePathNode();
                break;
            }
            if(p != path_node.at(path_node.size() - 1)){
                ImGui::SameLine();
            }
        }
    }

    void JViewExplorer::DrawMiddleHandle(){
        const float minWidth = 100.0f;    // Minimum allowable width
        const float maxWidth = 800.0f;   // Maximum allowable width
        const float splitterWidth = 8.0f; // Thickness of the draggable hit-box

        ImGui::SameLine();
        // Set cursor position to overlap slightly for a seamless look, then create hit-box
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x); 
        ImGui::InvisibleButton("v_splitter", ImVec2(splitterWidth, -1));

        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        if (ImGui::IsItemActive()) {
            // Adjust width based on mouse movement speed (io.MouseDelta)
            leftWidth += ImGui::GetIO().MouseDelta.x;
            
            // Clamp the values so the panels don't break or disappear
            if (leftWidth < minWidth) leftWidth = minWidth;
            if (leftWidth > maxWidth) leftWidth = maxWidth;
        }

        // 4. Right Panel
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x + splitterWidth);
    }

    void JViewExplorer::DrawLeftSide(){
        ImGui::Text("Left");
    }

    void JViewExplorer::DrawRightSide(){
        if(imgSize == 0){ // Line text
            int32_t c = 0;
            for(auto file : files){
                ImGui::Selectable(file.title.c_str(), c == selection);
                DrawItemTooltip(file);
                DrawItemEvent(file);
                c++;
            }
        }else{ // Grid Item
            ImGui::BeginGroup();
            float rightWidth = ImGui::GetWindowWidth();
            int32_t c = 0;
            int32_t row = std::floor<int32_t>(rightWidth / std::max<int32_t>(imgSize * 20 + 50, 50));
            for(auto file : files){
                ImGui::Button(file.title.c_str(), ImVec2(std::max<int32_t>(imgSize * 20 + 50, 50), std::max<int32_t>(imgSize * 20 + 50, 50)));
                DrawItemTooltip(file);
                DrawItemEvent(file);
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

        if(!target.is_dir){
            display_text += "Size: ";
            display_text += format_bytes(target.filesize);
            display_text += "\n";
        }

        ImGui::SetItemTooltip("%s", display_text.c_str());
    }

    void JViewExplorer::DrawItemEvent(JFileContent& target){
        fs::path root = jengine.context->project_path;
        std::string popup_id = "ViewExplorer_Right_Item_ContextItem_" + target.path.string();
        if(ImGui::BeginPopupContextItem(popup_id.c_str())){
            if (ImGui::MenuItem(("Find Reference In Scene##" + popup_id).c_str())){
                
            }
            ImGui::Separator();
            if (ImGui::MenuItem(("Delete##" + popup_id).c_str())){
                
            }
            ImGui::Separator();
            if (ImGui::MenuItem(("Cut##" + popup_id).c_str())){

            }
            if (ImGui::MenuItem(("Copy##" + popup_id).c_str())){

            }
            if (ImGui::MenuItem(("Paste##" + popup_id).c_str())){

            }
            ImGui::Separator();
            if (ImGui::MenuItem(("Copy Path##" + popup_id).c_str())){
                
            }
            if (ImGui::MenuItem(("Copy Relative Path##" + popup_id).c_str())){
                
            }
            ImGui::EndPopup();
        }
        if(ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Left)){

        }
        if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)){
            if(target.is_dir){
                std::lock_guard<std::mutex> guard(travel_record_mtx);
                changed = true;
                travel_record.push(path);
                path = fs::relative(target.path, root).string();
                spdlog::debug("Asset browse {}", path);
                UpdatePathNode();
            }
        }
    }

    void JViewExplorer::DrawRightSide_Event(){
        ImGuiPopupFlags background_flags = ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems;

        if(ImGui::BeginPopupContextWindow("ViewExplorer_Right_ContextItem", background_flags)){
            if (ImGui::MenuItem("Create Folder##ViewExplorer_Right_ContextItem")){
                
            }
            if (ImGui::MenuItem("Create Resource##ViewExplorer_Right_ContextItem")){
                
            }
            if (ImGui::MenuItem("Create Script##ViewExplorer_Right_ContextItem")){
                
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Open File Explorer Here##ViewExplorer_Right_ContextItem")){

            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut##ViewExplorer_Right_ContextItem")){

            }
            if (ImGui::MenuItem("Copy##ViewExplorer_Right_ContextItem")){

            }
            if (ImGui::MenuItem("Paste##ViewExplorer_Right_ContextItem")){

            }
            ImGui::Separator();
            if (ImGui::MenuItem("Copy Path##ViewExplorer_Right_ContextItem")){
                
            }
            if (ImGui::MenuItem("Copy Relative Path##ViewExplorer_Right_ContextItem")){
                
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Refresh##ViewExplorer_Right_ContextItem")){
                changed = true;
            }
            ImGui::EndPopup();
        }
    }

    void JViewExplorer::UpdatePathNode() {
        fs::path p = path;
        path_node.clear();
        while(p.has_parent_path()){
            path_node.push_back(p.filename().string());
            p = p.parent_path();
        }
        path_node.push_back(p.filename().string());
        std::reverse(path_node.begin(), path_node.end());
    }

    void JViewExplorer::ReloadProject(){
        path = "Assets";
        init = false;
    }

    fs::path JViewExplorer::CurrentFolder(){
        fs::path p = jengine.context->project_path;
        p /= path;
        return p;
    }
}