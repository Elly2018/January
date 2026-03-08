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
#include "viewbase.h"
#include <string>
#include <mutex>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#ifndef GUI_VIEW_CONSOLE_H
#define GUI_VIEW_CONSOLE_H

struct ImVec4;

namespace January::Engine::View {
    struct ConsoleLog {
        spdlog::level::level_enum level;
        std::string messages;
    };

    class JViewConsole : public JViewBase {
    public:
        DEFAULT_VIEW_CTOR(JViewConsole){}
        DEFAULT_VIEW_DECTOR(JViewConsole){}

        void Init() override;
        void DeInit() override;
        void Draw() override;
        void Update() override;

        void RenderBar();
        void RenderContent();

        const struct ImVec4 GetColor(spdlog::level::level_enum col);
        std::string GetName(spdlog::level::level_enum col);

    protected:
        void GetFilteredResult();
        void Clear();

    private:
        std::string search = "";
        spdlog::level::level_enum level_filter = spdlog::level::level_enum::info;
        std::vector<ConsoleLog> logs = std::vector<ConsoleLog>();
        std::vector<ConsoleLog> buffer = std::vector<ConsoleLog>();
        std::mutex buffer_mtx;
        bool changed = false;
        std::shared_ptr<spdlog::sinks::callback_sink_mt> callback_sink;
        std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
    };
}
#endif