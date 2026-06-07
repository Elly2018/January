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
#pragma once
#ifndef GUI_VIEW_SCRIPT_H
#define GUI_VIEW_SCRIPT_H
#include "../viewbase.h"
#include <atomic>
#include <string>
#include <memory>
#include <mutex>
#include <filesystem>
#include <TextEditor.h>

namespace fs = std::filesystem;

namespace January::Engine::View {

    struct ScriptContent {
        std::string uuid;
        fs::path path;
        bool dirty;
        std::string text = "";
    };

    class JViewScript : public JViewBase {
    public:
        DEFAULT_VIEW_CTOR(JViewScript) {}
        DEFAULT_VIEW_DECTOR(JViewScript) {}

        void Init() override;
        void Update() override;
        void Draw() override;
        void DeInit() override;
        void Focus(bool value) override;

    protected:
        void DrawTopBar();
        void DrawBottomBar();
        void DrawLeftList();
        void DrawRightContent();
        /**
         * @brief Left panel and right panel require splitter
         * And this function is that splitter
         * ImGui::SameLine is included, no need to write it
         */
        void DrawMiddleHandle();

        void OnChanged();
        void OnTransaction(std::vector<TextEditor::Change>& changed);
    private:
        /**
         * @brief Init will be the trigger
         * Define if the first event call is on.
         */
        std::atomic_bool init = false;
        /**
         * @brief Because left right panel is using splitter
         * The left width is dynamic which required a variable to record it
         */
        std::atomic<float> leftWidth = 0;
        TextEditor m_editorContext;
        bool m_isOpen = true;
        ScriptContent* m_currentFile = nullptr;
        std::atomic<float> fontSize = 17.0f;
        std::atomic<size_t> version = 0;
        std::vector<ScriptContent> files_buffer;
        std::mutex files_buffer_mtx;
        std::atomic_int32_t line;
        std::atomic_int32_t column;
        std::atomic_bool should_load = false;
    };
}
#endif