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
#include <misc/cpp/imgui_stdlib.h>
#include <clip.h>
#include "../../../engine/engine.h"
#include "../../../engine/struct/config.h"
#include "../../../engine/struct/context.h"
#include "../../../engine/utility/format.h"

namespace fs = std::filesystem;

namespace January::Engine::View {

    void openFolder(const std::string& path) {
        std::string command;

        #if defined(_WIN32) || defined(_WIN64)
            // Windows: use "explorer" command
            command = "explorer \"" + path + "\"";
        #elif defined(__APPLE__)
            // macOS: use "open" command
            command = "open \"" + path + "\"";
        #elif defined(__linux__)
            // Linux: use "xdg-open" (standard for most desktops)
            command = "xdg-open \"" + path + "\"";
        #else
            #error "Unsupported platform"
        #endif

        std::system(command.c_str());
    }

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
        folder_node.name = "Assets";
        folder_node.path = "Assets";
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
                {
                    std::lock_guard<std::mutex> guard(travel_record_mtx);
                    while(!travel_record.empty()){
                        travel_record.pop();
                    }
                }
                path = "Assets";
                path_node.clear();
                path_node.push_back("Assets");
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
            { // Top bar
                ImGui::BeginChild("ViewExplorer_Top", ImVec2(0, 0), ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoScrollbar);
                DrawPathAction();
                ImGui::SameLine();
                DrawPathBar();
                ImGui::EndChild();
            }
            {
                ImGui::BeginChild("ViewExplorer_Left", ImVec2(leftWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true, ImGuiWindowFlags_NoScrollbar);
                DrawLeftSide();
                ImGui::EndChild();
            }
            DrawMiddleHandle();
            {
                ImGui::BeginChild("ViewExplorer_Right", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);
                DrawRightSide();
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

    void JViewExplorer::DrawItemLine(JFileContent& target){
        ImGui::PushFont(jengine.context->icon_font);
        if(target.is_dir){
            ImGui::Text("%s", "\uf07b");
        }else{
            ImGui::Text("%s", "\uf15c");
        }
        ImGui::PopFont();
        ImGui::SameLine();
        ImGui::Selectable(target.title.c_str());
        DrawItemTooltip(target);
        DrawItemEvent(target);
    }

    void JViewExplorer::DrawItemGrid(JFileContent& target, int32_t size){
        ImGui::Button(target.title.c_str(), ImVec2(size, size));
        DrawItemTooltip(target);
        DrawItemEvent(target);
    }

    void JViewExplorer::DrawPathAction() {
        ImGui::PushFont(jengine.context->icon_font);
        if(path_input == PathBarDisplay::DEFAULT) {
            ImGui::BeginDisabled(travel_record.size() == 0);
            if(ImGui::Button("\uf053##Exploere_Icon_Action")){ // Return
                std::lock_guard<std::mutex> guard(travel_record_mtx);
                std::string buffer = travel_record.top();
                travel_record.pop();
                path = buffer;
                UpdatePathNode();
                changed = true;
            }
            ImGui::EndDisabled();
        }else{
            if(ImGui::Button("\uf053##Exploere_Icon_Action")){ // Return
                path_input = PathBarDisplay::DEFAULT;
            }
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(path_node.size() == 1 || path_input != PathBarDisplay::DEFAULT);
        if(ImGui::Button("\uf062##Exploere_Icon_Action")){ // Up
            std::lock_guard<std::mutex> guard(travel_record_mtx);
            fs::path buffer = path;
            buffer = buffer.parent_path();
            travel_record.push(path);
            path = buffer.string();
            spdlog::debug("Asset browse {}", path);
            UpdatePathNode();
            changed = true;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::BeginDisabled(path == "Assets" || path_input != PathBarDisplay::DEFAULT);
        if(ImGui::Button("\uf015##Exploere_Icon_Action")){ // Home
            std::lock_guard<std::mutex> guard(travel_record_mtx);
            travel_record.push(path);
            path = "Assets";
            spdlog::debug("Asset browse {}", path);
            UpdatePathNode();
            changed = true;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if(ImGui::Button("\uf121##Exploere_Icon_Action")){ // Input
            path_input = PathBarDisplay::PATH_INPUT;
        }
        if(mode == DisplayMode::NORMAL){
            ImGui::SameLine();
            if(ImGui::Button("\uf002##Search")){ // Search
                search = "";
                path_input = PathBarDisplay::SEARCH_INPUT;
            }
        }
        ImGui::PopFont();
    }

    void JViewExplorer::DrawPathBar() {
        if(path_input == PathBarDisplay::PATH_INPUT){
            std::string previous = path;
            if(ImGui::InputText("Path##Explorer_Path_InputText", &path, ImGuiInputTextFlags_EnterReturnsTrue)){
                fs::path sss = jengine.context->project_path;
                sss /= path;
                if(fs::exists(sss) && fs::is_directory(sss)){
                    std::lock_guard<std::mutex> guard(travel_record_mtx);
                    travel_record.push(previous);
                    spdlog::debug("Asset browse {}", path);
                    UpdatePathNode();
                    path_input = PathBarDisplay::DEFAULT;
                    changed = true;
                }else{
                    spdlog::error("Enter path is not vaild {}", path);
                    path_input = PathBarDisplay::DEFAULT;
                    path = previous;
                }
            }
        }
        else if(path_input == PathBarDisplay::SEARCH_INPUT) {
            if(ImGui::InputText("Path##Explorer_Path_InputText", &search, ImGuiInputTextFlags_EnterReturnsTrue)){
                StartSearch();
            }
        }
        else{
            fs::path buffer = "";
            for(auto& p : path_node){
                bool not_last = p != path_node.at(path_node.size() - 1);
                buffer /= p;
                ImGui::BeginDisabled(!not_last);
                if(ImGui::Button((p + "##Explorer_Path_Button").c_str())){
                    std::lock_guard<std::mutex> guard(travel_record_mtx);
                    travel_record.push(path);
                    path = buffer;
                    spdlog::debug("Asset browse {}", path);
                    UpdatePathNode();
                    changed = true;
                }
                ImGui::EndDisabled();
                if(not_last){
                    ImGui::SameLine();
                }
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

    void JViewExplorer::DrawLeftSideTreeNode(JFolderContent& tree, int32_t level){
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 14.0f);
        bool leaf = tree.children.size() == 0 && tree.is_open;
        ImGuiTreeNodeFlags tree_flag = ImGuiTreeNodeFlags_None;
        if(leaf) {
            tree_flag |= ImGuiTreeNodeFlags_Leaf;
        }
        if(ImGui::TreeNodeEx((tree.name + "##Exploere_Left_Panel_Tree_Node_" + std::to_string(level)).c_str(), tree_flag)){
            if (ImGui::IsItemToggledOpen()) {
                {
                    fs::path pp = jengine.context->project_path;
                    tree.is_open = true;
                    pp /= tree.path;
                    spdlog::debug("\tStart fetch files...");
                    tree.children.clear();
                    for(auto entry : fs::directory_iterator(pp)){
                        if(!entry.is_directory()){
                            continue;
                        }
                        spdlog::debug("\t\tDetect entry: {}", entry.path().c_str());
                        JFolderContent* folder = new JFolderContent();
                        folder->path = tree.path + "/" + entry.path().filename().string();
                        folder->name = entry.path().filename();
                        folder->is_open = false;
                        tree.children.push_back(folder);
                        spdlog::info("Assgin file watch event to {}", entry.path().string().c_str());
                    }
                }
            }
            for(auto child : tree.children){
                DrawLeftSideTreeNode(*child, level + 1);
            }
            ImGui::TreePop();
        }
        ImGui::PopStyleVar();
    }

    void JViewExplorer::DrawLeftSide(){
        DrawLeftSideTreeNode(folder_node, 1);
    }

    void JViewExplorer::DrawRightSide(){
        ImVec2 availSpace = ImGui::GetContentRegionAvail();
        float h = availSpace.y;
        { // Top
            ImGui::BeginChild("Exploere_Right_Panel_Top", ImVec2(0, h - (ImGui::GetTextLineHeightWithSpacing() * 1.2f)));
            DrawRightSide_Event();
            if(imgSize == 0){ // Line text
                int32_t c = 0;
                for(auto file : files){
                    if(!FilterCheck(file)) continue;
                    DrawItemLine(file);
                    c++;
                }
            }else{ // Grid Item
                ImGui::BeginGroup();
                float rightWidth = ImGui::GetWindowWidth();
                int32_t c = 0;
                int32_t max = imgSize * 10 + 100;
                int32_t row = std::max<int32_t>(std::floor<int32_t>(rightWidth / max), 1);
                for(auto file : files){
                    if(!FilterCheck(file)) continue;
                    DrawItemGrid(file, max);
                    if((c + 1) % row != 0){
                        ImGui::SameLine();
                    }
                    c++;
                }
                ImGui::EndGroup();
            }
            ImGui::EndChild();
        }
        { // Bottom
            ImGui::BeginChild("Exploere_Right_Panel_Bottom", ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 1.2f), ImGuiBackendFlags_None, ImGuiWindowFlags_NoScrollbar);
            int32_t temp_imgSize = imgSize.load();
            ImGui::SliderInt("size", &temp_imgSize, 0, 10, "%d");
            imgSize.store(temp_imgSize);
            ImGui::EndChild();
        }
    }

    void JViewExplorer::DrawItemTooltip(JFileContent& target){
        std::string display_text = "";
        display_text += "Filename: ";
        display_text += target.path.filename().string().c_str();
        display_text += "\n";

        display_text += "Folder: ";
        display_text += fs::relative(target.path.parent_path(), jengine.context->project_path).string();
        display_text += "\n";

        display_text += "Fullpath: ";
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
            if(target.is_dir){
                if (ImGui::MenuItem(("Open File Explorer Here##" + popup_id).c_str())){
                    openFolder(target.path);
                }
            }else{
                if (ImGui::MenuItem(("Find Reference In Scene##" + popup_id).c_str())){
                }
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
                clip::set_text(target.path.string());
            }
            if (ImGui::MenuItem(("Copy Relative Path##" + popup_id).c_str())){
                clip::set_text(fs::relative(target.path, root).string());
            }
            ImGui::EndPopup();
        }
        if(ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Left)){

        }
        if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)){
            if(target.is_dir){
                std::lock_guard<std::mutex> guard(travel_record_mtx);
                travel_record.push(path);
                path = fs::relative(target.path, root).string();
                spdlog::debug("Asset browse {}", path);
                UpdatePathNode();
                changed = true;
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
                openFolder(CurrentFolder().string());
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
                clip::set_text(CurrentFolder().string());
            }
            if (ImGui::MenuItem("Copy Relative Path##ViewExplorer_Right_ContextItem")){
                clip::set_text(fs::relative(CurrentFolder(), jengine.context->project_path).string());
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

    void JViewExplorer::StartSearch() {

    }

    bool JViewExplorer::FilterCheck(JFileContent& file) {
        if(filter == FilterFlag::NONE) return true;
        return false;
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