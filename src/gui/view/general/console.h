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
#ifndef GUI_VIEW_CONSOLE_H
#define GUI_VIEW_CONSOLE_H
#include "../viewbase.h"
#include <string>
#include <mutex>
#include <spdlog/spdlog.h>
#include "../../../engine/utility/logger.h"

struct ImVec4;

namespace January::Engine::View {
    //
    // The console view
    // Handle viewer for background logger informations
    //
    class JViewConsole : public JViewBase {
    public:
        DEFAULT_VIEW_CTOR(JViewConsole){}
        DEFAULT_VIEW_DECTOR(JViewConsole){}

        void Init() override;
        void DeInit() override;
        void Draw() override;
        void Update() override;

    protected:
        // Render top toolbar
        void DrawBar();
        // Render bottom content area
        void DrawContent();
        void DrawDetail();
        void DrawMiddleHandle(float total_window_height, float splitterHeight);

        // Get logger level color
        const struct ImVec4 GetColor(spdlog::level::level_enum col);
        // Get logger level prefix
        std::string GetName(spdlog::level::level_enum col);

        // Get filtered data into "buffer" variable
        void GetFilteredResult();
        // Clear the logger list
        // This will clear the logger record in the global state as well
        void Clear();
        // Get current logger instance
        JLoggerWorker* GetLogger();

    private:
        // Current logger selection
        // 0: Engine logger
        // 1: Runtime logger
        // 2: Script logger
        int32_t logger_index = -1;
        // Init will be the trigger
        // Define if the first event call is on.
        std::atomic_bool init = false;
        // Check user have change the filter or change the logger to view or not
        // When it's true, the update get called at next frame
        std::atomic_bool change_page = false;
        // The toggle which when new log added
        // Scroll the list view to the very bottom
        std::atomic_bool auto_scroll_next = false;
        std::atomic_int32_t open_bottom = -1;
        std::string search = "";
        std::atomic<float> topHeight = 0;
        std::vector<JConsoleLog> buffer = std::vector<JConsoleLog>();
        enum spdlog::level::level_enum level_filter;
        std::mutex buffer_mtx;
    };
}
#endif