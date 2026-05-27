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
#ifndef ENGINE_UTILITY_LOGGER_H
#define ENGINE_UTILITY_LOGGER_H
#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace January::Engine { 
    //
    // The log record from the spdlog
    //
    struct JConsoleLog {
        uint32_t id;
        spdlog::level::level_enum level;
        std::string messages;
    };

    class JLoggerWorker {
    public:
        //
        // Argument:
        // * name: The logger name prefix
        // * global: Should be merge to global spdlog function
        // * level: logger level
        //
        JLoggerWorker(const char* name, bool global, enum spdlog::level::level_enum _level);
        ~JLoggerWorker();

        std::vector<JConsoleLog> logs = std::vector<JConsoleLog>();   
        std::mutex log_mtx;
        std::shared_ptr<spdlog::sinks::callback_sink_mt> callback_sink;
        uint32_t id_counter;
        static spdlog::logger*      logger;
        enum spdlog::level::level_enum level;
        // Check if logger vector is changed
        // Use flip to make change variable false, some sort of marker or update
        bool IsChanged(bool flip = true);
        bool IsGlobal();
    private:
        bool changed;
        bool isglobal;
    };

    //
    // This is for console view content output
    // Everything called here can be find in console view
    // spdlog::info <- default logger is for os cli console output
    //
    struct JLogger final {
    public:
        JLogger();
        ~JLogger();

        // Engine level logger
        // Vulkan stuff or application or thread output stuff
        JLoggerWorker*      logger;
        // Only for runtime, such as entt or scene, composition
        JLoggerWorker*      runtime_logger;
        // Gravity script logger
        // Mostly for user
        JLoggerWorker*      script_logger;
    };
}

#endif