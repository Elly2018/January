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
#include "blueprint.h"
#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui_node_editor.h>
#include "../../../engine/utility/path.h"
#include "../../nodes/nodebase.h"
#include "../../nodes/edgebase.h"

namespace ed = ax::NodeEditor;

namespace January::Engine::View {
    ImTextureID m_HeaderBackground;
    ImTextureID m_SaveIcon;
    ImTextureID m_RestoreIcon;

    void JViewBlueprint::Init(){
        spdlog::info("Loaded View: Blueprint");
        CreateTempConfig();
        BlueprintObject* bo = blueprints.at(blueprints.size() - 1);
        spdlog::debug("Load temp file: {}", bo->config->SettingsFile);
        //m_HeaderBackground = LoadTexture("data/BlueprintBackground.png");
        //m_SaveIcon         = LoadTexture("data/ic_save_white_24dp.png");
        //m_RestoreIcon      = LoadTexture("data/ic_restore_white_24dp.png");
    }
    void JViewBlueprint::Update(){

    }
    void JViewBlueprint::Draw(){
        ImGuiIO& io = ImGui::GetIO();

        if(ImGui::BeginTabBar("BlueprintTab#jblueprint")){
            for(auto& b : blueprints){
                if(ImGui::BeginTabItem((b->file + "#jblueprint_tab_title").c_str())){
                    DrawBlueprint(*b);
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    }
    void JViewBlueprint::DeInit(){
        for(auto& b : blueprints){
            ed::DestroyEditor(b->ctx);
            delete b->config;
            for(auto& n : b->nodes){
                delete n;
            }
            for(auto& e : b->edges){
                delete e;
            }
            delete b;
        }
        blueprints.clear();
    }

    void JViewBlueprint::DrawBlueprint(BlueprintObject& bo){
        ed::SetCurrentEditor(bo.ctx);
        ed::Begin("Blueprint Editor");
        {
            ImVec2 cursorTopLeft = ImGui::GetCursorScreenPos();
            //util::BlueprintNodeBuilder builder(m_HeaderBackground, GetTextureWidth(m_HeaderBackground), GetTextureHeight(m_HeaderBackground));
            int32_t uniqueId = 0;

            for(auto& n : bo.nodes){
                n->PreDraw(uniqueId);
                n->Draw();
                n->PostDraw();
            }

            for(auto& e : bo.edges){

            }
        }
        ed::End();
        if(ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered()){
            ed::ShowBackgroundContextMenu();
        }
        ed::SetCurrentEditor(nullptr);
    }

    void JViewBlueprint::CreateTempConfig(){
        fs::path tempFile = get_temp_directory();
        tempFile /= "placeholder.json";
        if(fs::exists(tempFile)) fs::remove(tempFile);
        blueprints.push_back(new BlueprintObject());
        BlueprintObject* bo = blueprints.at(blueprints.size() - 1);
        bo->config = new ed::Config();
        bo->config->SettingsFile = tempFile.string().c_str();
        bo->file = "temp";
        bo->path = tempFile;
        bo->ctx = ed::CreateEditor(bo->config);
    }
}