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
#include <memory>
#include "view/manager.h"
#include "struct/config.h"
#include "struct/context.h"
#include "../system/window.h"

namespace January::Engine {
    using namespace View;

    // Save app config to app preference location
    void SaveAppConfig(AppConfig& target);
    // Load app config from app preference location
    void LoadAppConfig(AppConfig& config);
    // Generate a app context data struct
    void GenerateAppContext(AppContext& ctx);

    struct JEngine {
        AppConfig                   config;
        AppContext                  context;
        ViewManager                 manager;
    };

    // Engine init
    int32_t EngineInit(JEngine& jengine, JWindow& jwindow);
    // Engine release memory
    void EngineDeInit(JEngine& jengine);
    // Update engine logic and stuff
    void EngineUpdate(JEngine& jengine);
    // Render engine GUI and scene
    void EngineDraw(JEngine& jengine, JWindow& jwindow);
}