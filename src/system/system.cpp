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
#include "../gui/manager.h"
#include "../engine/engine.h"
#include "../engine/struct/context.h"

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
        if(err != 0){
            spdlog::error("Vulkan Init Error");
            return err;
        }
        err = EngineInit(*jsystem.engine, *jsystem.window);
        if(err != 0){
            spdlog::error("Engine Init Error");
            return err;
        }
        VInit(*jsystem.engine->manager, jsystem);
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
            std::queue<std::string>& cb = jsystem.engine->context->commands;
            if(cb.size() > 0){
                std::string command = cb.front();
                cb.pop();
            }
            if(jsystem.window->g_done){
                jsystem.engine->context->done = true;
            }
        }
        jsystem.window->g_done = true;
        if(draw_thread.joinable()) draw_thread.join();
        if(update_thread.joinable()) update_thread.join();
    }
}