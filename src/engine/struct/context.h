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
#ifndef ENGINE_STRUCT_CONTEXT_H
#define ENGINE_STRUCT_CONTEXT_H
#include <string>
#include <queue>
#include <mutex>

struct ImFont;

namespace spdlog {
    struct logger;
}

namespace January::Engine {
    // Current application context
    // This data cannot be store in disk, This is the memory only data
    struct AppContext {
        std::string                 project_path                  = "";
        // Does application needs load project right now
        bool                        load_project                  = false;
        // Application global time
        double                      time                          = 0;
        // Application delta time
        double                      delta                         = 0;
        // Application end signal
        bool                        done                          = false;
        // Command buffer, execute next frame
        std::queue<std::string>     commands                      = std::queue<std::string>();
        std::mutex                  commands_mtx;
        struct ImFont*              text_font;
        struct ImFont*              icon_font;
        struct ImFont*              emoji_font;
        // This is for console view content output
        // Everything called here can be find in console view
        // spdlog::info <- default logger is for os cli console output
        struct spdlog::logger*      logger;
        //
        
    };
}
#endif