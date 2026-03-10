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
#include "system.h"
#include <thread>
#include <spdlog/spdlog.h>
#include "window.h"
#include "../engine/engine.h"
#include "../engine/struct/context.h"
#include "../engine/utility/command.h"
#include "../gui/manager.h"
#include "../gui/view/explorer.h"

namespace January::System {
    using namespace Engine;

    void UpdateLoop(JSystem& jsystem){
        while(!jsystem.window->g_done){
            EngineUpdate(*jsystem.engine);
        }
    }

    int32_t SInit(JSystem& jsystem){
        jsystem.window = new JWindow();
        jsystem.engine = new Engine::JEngine();
        int32_t err = JInit(*jsystem.window, JRWindowInit());
        assert(err == 0 && "Vulkan Init Error");
        err = EngineInit(*jsystem.engine, *jsystem.window, jsystem);
        assert(err == 0 && "Engine Init Error");
        LoadAppConfig(*jsystem.engine->config);
        ApplyEnableConfig(*jsystem.engine->manager, *jsystem.engine->config);
        LoadPreference();
        return 0;
    }

    void SRun(JSystem& jsystem){
        spdlog::debug("Enter Application Mainloop");
        std::thread draw_thread([&jsystem]() {
            DrawLoop(jsystem);
        });
        std::thread update_thread([&jsystem](){
            UpdateLoop(jsystem);
        });

        while(!jsystem.engine->context->done){
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            // [If using SDL_MAIN_USE_CALLBACKS: call ImGui_ImplSDL3_ProcessEvent() from your SDL_AppEvent() function]
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL3_ProcessEvent(&event);
                if (event.type == SDL_EVENT_QUIT)
                    jsystem.window->g_done = true;
                if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(jsystem.window->g_window))
                    jsystem.window->g_done = true;
            }

            {
                std::lock_guard<std::mutex> lock(jsystem.engine->context->commands_mtx);
                std::queue<std::string>& cb = jsystem.engine->context->commands;
                while(cb.size() > 0){
                    std::string command = cb.front();
                    cb.pop();
                    ApplyCommand(jsystem, command);
                }
            }
            if(jsystem.engine->context->load_project){
                jsystem.engine->manager->explorer->ReloadProject();
                jsystem.engine->context->load_project = false;
            }
            if(jsystem.window->g_done){
                jsystem.engine->context->done = true;
            }
        }
        jsystem.window->g_done = true;
        if(draw_thread.joinable()) draw_thread.join();
        if(update_thread.joinable()) update_thread.join();
        EngineDeInit(*jsystem.engine);
        JDeInit(*jsystem.window);

        delete jsystem.engine;
        delete jsystem.window;
    }
}