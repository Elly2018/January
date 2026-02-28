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
#include <spdlog/spdlog.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#ifndef GUI_VIEW_CONSOLE_H
#define GUI_VIEW_CONSOLE_H

namespace January::Engine::View {
    struct ConsoleLog {
        spdlog::level::level_enum level;
        std::vector<std::string> messages;
    };

    class JViewConsole : public JViewBase {
    public:
        DEFAULT_VIEW_CTOR(JViewConsole) {
            callback_sink = std::make_shared<spdlog::sinks::callback_sink_mt>([](const spdlog::details::log_msg &msg) {
                // for example you can be notified by sending an email to yourself
            });
            callback_sink->set_level(spdlog::level::info);
            console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            logger = new spdlog::logger("engine logger", {console_sink, callback_sink});
        }
        DEFAULT_VIEW_DECTOR(JViewConsole) {
            delete logger;
            console_sink.reset();
            callback_sink.reset();
        }

        void Draw() override;
        void Update() override;

        void RenderBar();
        void RenderContent();

        spdlog::logger& GetLogger() { return *logger; }

    private:
        std::vector<ConsoleLog> logs = std::vector<ConsoleLog>();
        std::shared_ptr<spdlog::sinks::callback_sink_mt> callback_sink;
        std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink;
        spdlog::logger* logger;
    };
}
#endif