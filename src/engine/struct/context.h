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
#include <unordered_map>
#include <atomic>
#include <memory>
#include <string>
#include <queue>
#include <mutex>

struct ImFont;

namespace spdlog
{
    struct logger;
}

namespace January::Engine
{
    class JAssetWorker;
    class JLogger;
    struct JAssetBase;

    using Assets = std::vector<std::shared_ptr<JAssetBase>>;

    /**
     * @brief Current application context
     * This data cannot be store in disk, This is the memory only data
     * 
     * Content could be access by Update and Render at the same time. in order to avoid
     * memory issue, We have atomic and mutex everywhere in this struct
     */
    struct AppContext
    {
        AppContext() = default;
        ~AppContext() = default;
        
        std::string project_path = "";
        std::mutex project_path_mtx;
        // Does application needs load project right now
        std::atomic_bool load_project = false;
        // Application global time
        std::atomic<double> time = 0;
        std::atomic<double> runtime_time = 0;
        // Application delta time
        std::atomic<double> delta = 0;
        std::atomic<double> runtime_delta = 0;
        std::atomic_bool runtime_state = false;
        // Application end signal
        std::atomic_bool done = false;
        // Command buffer, execute next frame
        // In order to use this in thread-safe
        // Please lock_guard "commands_mtx" before add item or pop item to it
        std::queue<std::string> commands = std::queue<std::string>();
        std::mutex commands_mtx;
        // Imgui defualt text
        ImFont* text_font;
        // Imgui font with icon like graph
        ImFont* icon_font;
        // Imgui font with emoji like graph
        ImFont* emoji_font;
        // Logger
        std::unique_ptr<JLogger> logger;
        // Asset
        std::unique_ptr<JAssetWorker> asset;
        // Select asset
        Assets asset_selection;
        std::mutex asset_selection_mtx;
    };
}
#endif