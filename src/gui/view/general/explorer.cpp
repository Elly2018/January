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
        UpdateFileWatcher();
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
        DrawItemTooltip(target.path, target.is_dir, target.filesize);
        DrawItemEvent(target.path, target.is_dir);
    }

    void JViewExplorer::DrawItemGrid(JFileContent& target, int32_t size){
        ImGui::Button(target.title.c_str(), ImVec2(size, size));
        DrawItemTooltip(target.path, target.is_dir, target.filesize);
        DrawItemEvent(target.path, target.is_dir);
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
        ImGui::BeginDisabled(path_input != PathBarDisplay::DEFAULT);
        if(ImGui::Button("\uf120##Exploere_Icon_Action")){ // Input
            path_input = PathBarDisplay::PATH_INPUT;
            path_buffer = path;
        }
        if(mode == DisplayMode::NORMAL){
            ImGui::SameLine();
            if(ImGui::Button("\uf002##Search")){ // Search
                search = "";
                path_input = PathBarDisplay::SEARCH_INPUT;
                search_state = SearchState::NONE;
            }
        }
        ImGui::EndDisabled();
        ImGui::PopFont();
    }

    void JViewExplorer::DrawPathBar() {
        if(path_input == PathBarDisplay::PATH_INPUT){
            if(ImGui::InputText("Path##Explorer_Path_InputText", &path_buffer, ImGuiInputTextFlags_EnterReturnsTrue)){
                fs::path sss = jengine.context->project_path;
                sss /= path_buffer;
                if(fs::exists(sss) && fs::is_directory(sss)){
                    std::lock_guard<std::mutex> guard(travel_record_mtx);
                    travel_record.push(path);
                    path = path_buffer;
                    spdlog::debug("Asset browse {}", path);
                    UpdatePathNode();
                    path_input = PathBarDisplay::DEFAULT;
                    changed = true;
                }else{
                    spdlog::error("Enter path is not vaild {}", path);
                    path_input = PathBarDisplay::DEFAULT;
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
        fs::path pp = jengine.context->project_path;
        pp /= tree.path;
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 14.0f);
        bool leaf = tree.children.size() == 0 && tree.is_open;
        bool selected = CurrentFolder() == pp;
        ImGuiTreeNodeFlags tree_flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
        if(leaf) {
            tree_flag |= ImGuiTreeNodeFlags_Leaf;
        }
        if(selected) {
            tree_flag |= ImGuiTreeNodeFlags_Selected;
        }
        if(ImGui::TreeNodeEx((tree.name + "##Exploere_Left_Panel_Tree_Node_" + pp.string() + "__" + std::to_string(level)).c_str(), tree_flag)){
            DrawItemEvent(pp, true, true);
            if (ImGui::IsItemToggledOpen()) {
                if(!tree.is_open){
                    tree.is_open = true;
                    spdlog::debug("\tStart fetch files...");
                    tree.children.clear();
                    for(auto entry : fs::directory_iterator(pp)){
                        if(!entry.is_directory()) continue;
                        spdlog::debug("\t\tDetect folder entry: {}", entry.path().c_str());
                        JFolderContent* folder = new JFolderContent();
                        folder->path = tree.path + "/" + entry.path().filename().string();
                        folder->name = entry.path().filename();
                        folder->is_open = false;
                        spdlog::debug("\t\tAssign folder path: {}", folder->path);
                        std::lock_guard<std::mutex> lock(folder_node_mtx);
                        tree.children.push_back(folder);
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
            if(mode == DisplayMode::NORMAL) DrawRightSide_Event();
            if(imgSize == 0){ // Line text
                int32_t c = 0;
                std::lock_guard<std::mutex> guard(files_mtx);
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
                int32_t row = std::max<int32_t>(std::floor<int32_t>(rightWidth / max) - 1, 1);
                std::lock_guard<std::mutex> guard(files_mtx);
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

    void JViewExplorer::DrawItemTooltip(fs::path _path, bool is_dir, uintmax_t filesize){
        std::string display_text = "";
        display_text += "Filename: ";
        display_text += _path.filename().string().c_str();
        display_text += "\n";

        display_text += "Folder: ";
        display_text += fs::relative(_path.parent_path(), jengine.context->project_path).string();
        display_text += "\n";

        display_text += "Fullpath: ";
        display_text += _path.parent_path().string().c_str();
        display_text += "\n";

        if(!is_dir){
            display_text += "Size: ";
            display_text += format_bytes(filesize);
            display_text += "\n";
        }

        ImGui::SetItemTooltip("%s", display_text.c_str());
    }

    void JViewExplorer::DrawItemEvent(fs::path _path, bool is_dir, bool tree_node){
        fs::path root = jengine.context->project_path;
        std::string popup_id = "ViewExplorer_Right_Item_ContextItem_" + _path.string() + "__" + std::to_string(is_dir) + "__" + std::to_string(tree_node);
        if(ImGui::BeginPopupContextItem(popup_id.c_str())){
            if(is_dir){
                if (ImGui::MenuItem(("Open File Explorer Here##" + popup_id).c_str())){
                    openFolder(_path);
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
                clip::set_text(_path.string());
            }
            if (ImGui::MenuItem(("Copy Relative Path##" + popup_id).c_str())){
                clip::set_text(fs::relative(_path, root).string());
            }
            ImGui::EndPopup();
        }
        bool tree_node_single = tree_node && ImGui::IsItemHovered() && ImGui::IsItemClicked(ImGuiMouseButton_Left);
        bool none_tree_double = !tree_node && is_dir && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
        if(tree_node_single || none_tree_double){
            std::lock_guard<std::mutex> guard(travel_record_mtx);
            travel_record.push(path);
            path = fs::relative(_path, root).string();
            spdlog::debug("Asset browse {}", path);
            spdlog::debug("\tAsset browse root: {}", root.string());
            spdlog::debug("\tAsset browse path: {}", path);
            spdlog::debug("\tAsset browse _path: {}", _path.string());
            UpdatePathNode();
            changed = true;
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
        if(search_state == SearchState::SEARCHING){

        }
    }

    bool JViewExplorer::FilterCheck(JFileContent& file) {
        if(filter == FilterFlag::NONE) return true;
        if(file.is_dir) return true;
        std::string ext = file.path.extension().string();
        if(ext == ".janunary_prefab" && ((int32_t)filter.load() & (int32_t)FilterFlag::PREFAB) != 0) return true;
        if(ext == ".janunary_material" && ((int32_t)filter.load() & (int32_t)FilterFlag::MATERIAL) != 0) return true;
        if(ext == ".janunary_texture" && ((int32_t)filter.load() & (int32_t)FilterFlag::TEXTURE) != 0) return true;
        return false;
    }

    void JViewExplorer::UpdateFileWatcher() {
        if(!changed) return;
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

            {
                std::lock_guard<std::mutex> guard(files_mtx);
                files.clear();
            }
            spdlog::debug("\tClean file data");
            UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
            {
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
                    {
                        std::lock_guard<std::mutex> guard(files_mtx);
                        files.push_back(file);
                    }

                    spdlog::debug("Assgin file watch event to {}", entry.path().string().c_str());
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

    void JViewExplorer::ReloadProject(){
        path = "Assets";
        init = false;
    }

    fs::path JViewExplorer::CurrentFolder(){
        fs::path p = jengine.context->project_path;
        p /= path;
        return p;
    }

    fs::path JViewExplorer::CurrentMetaFolder() {
        fs::path p = jengine.context->project_path;
        p /= ".january";
        p /= path;
        return p;
    }
}