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
#include "script.h"
#include <functional>
#include <spdlog/spdlog.h>
#include <uuid_v4/uuid_v4.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include "../../../engine/engine.h"
#include "../../../engine/struct/context.h"

namespace January::Engine::View {
    void JViewScript::Init() {
        JViewBase::Init();
        spdlog::info("Loaded View: Script");
        m_editorContext.SetLanguage(TextEditor::Language::AngelScript());
        m_editorContext.SetChangeCallback(std::bind(&JViewScript::OnChanged, this));
        m_editorContext.SetTransactionCallback(std::bind(&JViewScript::OnTransaction, this, std::placeholders::_1));
        window_flag |= ImGuiWindowFlags_MenuBar;
    }

    void JViewScript::Update() {
        int l; 
        int c;
        m_editorContext.GetCurrentCursor(l, c);
        line.store(l, std::memory_order_relaxed);
        column.store(c, std::memory_order_relaxed);
    }

    void JViewScript::Draw() {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiIO& io = ImGui::GetIO();
        float w = std::max(ImGui::GetContentRegionAvail().x, 20.f);
        if(!init){
            init = true;
            leftWidth = w * (1.f / 10.f);
        }

        DrawTopBar();
        DrawBottomBar();
        {
            ImGui::BeginChild("ViewExplorer_Left", ImVec2(leftWidth - (style.DisplayWindowPadding.x / 1.5f), 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
            DrawLeftList();
            ImGui::EndChild();
        }
        DrawMiddleHandle();
        {
            ImGui::BeginChild("ViewExplorer_Right", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);
            DrawRightContent();
            ImGui::EndChild();
        }
    }

    void JViewScript::DeInit() {
        
    }

    void JViewScript::Focus(bool value) {
        
    }

    void JViewScript::DrawTopBar(){
        if(ImGui::BeginMenuBar()){
            if(ImGui::BeginMenu("File")){
                if(ImGui::MenuItem("New File")){
                    {
                        std::lock_guard<std::mutex> lock(files_buffer_mtx);
                        
                        ScriptContent sc = ScriptContent();
                        UUIDv4::UUIDGenerator<std::string> generate = UUIDv4::UUIDGenerator<std::string>();
                        sc.uuid = generate.getUUID().str();
                        sc.path = "";
                        sc.dirty = true;
                        files_buffer.push_back(sc);
                    }
                }
                ImGui::Separator();
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Edit")){
                if(ImGui::MenuItem("Cut")){

                }
                if(ImGui::MenuItem("Copy")){

                }
                if(ImGui::MenuItem("Paste")){

                }
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Help")){
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    void JViewScript::DrawBottomBar(){
        if(ImGui::BeginChild("Statu_bar##Script_Status_Bar", ImVec2(0, 36), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoScrollbar)){
            ImGui::Text("Line: %i, Col: %i", line.load(), column.load());
            ImGui::EndChild();
        }
    }

    void JViewScript::DrawLeftList(){
        {
            std::lock_guard<std::mutex> lock(files_buffer_mtx);
            for(auto& file : files_buffer){
                std::string display_name = file.path.filename().string();
                if(display_name.size() == 0){
                    display_name += "untitled";
                }
                if(file.dirty){
                    display_name += "*";
                }
                if(ImGui::Selectable(display_name.c_str(), m_currentFile != nullptr && m_currentFile->uuid == file.uuid)){
                    m_currentFile = &file;
                    m_editorContext.SetText(file.text);
                    should_load.store(true, std::memory_order_release);
                }
            }
        }
    }

    void JViewScript::DrawRightContent(){
        ImGui::PushFont(jengine.context->code_font, fontSize);
        m_editorContext.Render("TextEdit");
        ImGui::PopFont();
    }

    void JViewScript::DrawMiddleHandle(){
        const float minWidth = 100.0f;    // Minimum allowable width
        const float maxWidth = 800.0f;   // Maximum allowable width
        const float splitterWidth = 8.0f; // Thickness of the draggable hit-box

        ImGui::SameLine();
        // Set cursor position to overlap slightly for a seamless look, then create hit-box
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().ItemSpacing.x); 
        ImGui::InvisibleButton("v_splitter##Explorer_Splitter", ImVec2(splitterWidth, -1));

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

    void JViewScript::OnChanged(){
        if(m_currentFile != nullptr){
            m_currentFile->dirty = true;
        }
    }

    void JViewScript::OnTransaction(std::vector<TextEditor::Change>& changed){

    }
}