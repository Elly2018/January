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
#ifndef ENGINE_ENGINE_H
#define ENGINE_ENGINE_H
#include <cinttypes>
#include <filesystem>

namespace January {
    namespace System {
        struct JSystem;
        struct JWindow;
    }
    namespace Engine {
        struct AppConfig;
        struct AppContext;

        namespace View {
            struct ViewManager;
        }
        std::filesystem::path get_config_path(const char* path);
        // Save app config to app preference location
        void SaveAppConfig(struct AppConfig& target);
        // Load app config from app preference location
        void LoadAppConfig(struct AppConfig& config);
        // Generate a app context data struct
        void GenerateAppContext(struct AppContext& ctx);

        /**
         * @brief The core january engine context container. 
         */
        struct JEngine 
        {
            JEngine() = default;
            ~JEngine() = default;
            struct AppConfig*                  config;
            struct AppContext*                 context;
            struct View::ViewManager*          manager;
        };

        // Engine init
        int32_t EngineInit(JEngine& jengine, struct System::JWindow& jwindow, struct System::JSystem& system);
        // Engine release memory
        void EngineDeInit(JEngine& jengine);
        // Update engine logic and stuff
        void EngineUpdate(JEngine& jengine);

        // ...
        void AddRecent(JEngine& jengine, std::string& path);
        void RemoveRecent(JEngine& jengine, std::string& path);
    }
}
#endif