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
#include "inspector.h"
#include <mutex>
#include <spdlog/spdlog.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "../../../engine/engine.h"
#include "../../../engine/struct/context.h"
#include "../../../engine/utility/logger.h"
#include "../../../engine/assets/asset.h"

namespace January::Engine::View {
    void JViewInspector::Init() {
        JViewBase::Init();
        spdlog::info("Loaded View: Inspector");
    }

    void JViewInspector::Update() {

    }

    void JViewInspector::Draw() {
        std::shared_ptr<JAssetBase> select = nullptr;
        {
            std::lock_guard<std::mutex> lock(jengine.context->asset_selection_mtx);
            if(jengine.context->asset_selection.size() > 0){
                select = jengine.context->asset_selection.at(0);
            }
        }
        
        if(select != nullptr){
            ImGui::Text("UUID: %s", select->uuid.c_str());
            ImGui::Text("Target: %s", select->target.string().c_str());
            ImGui::Text("Meta Target: %s", select->meta_target.string().c_str());
        }
    }

    void JViewInspector::DeInit() {

    }

    void JViewInspector::Focus(bool value) {

    }
}