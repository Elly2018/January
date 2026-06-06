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
#ifndef ENGINE_SCRIPT_VM_H
#define ENGINE_SCRIPT_VM_H
#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <memory>
#include <angelscript.h>
#include "../utility/mutex.h"

namespace January {
    namespace System {
        struct JWindow;
    }
    namespace Engine {
        struct JEngine;

        struct ASEngineDeleter {
            void operator()(asIScriptEngine* engine) const {
                if (engine) {
                    engine->ShutDownAndRelease();
                }
            }
        };
        
        class AngelVM {
        public:
            AngelVM(System::JWindow& _win, JEngine& _engine);
            virtual ~AngelVM();

            bool IsCompiling();
            void UpdateVMContent();

        protected:
            void Compile();
            void ScriptMessageCallback(const asSMessageInfo* msg);
            
        protected:
            bool compiling = false;
            System::JWindow&    jwindow;
            JEngine&            jengine;
            std::unique_ptr<asIScriptEngine, ASEngineDeleter> engine;

            std::vector<std::string> modules;
            JMUTEX(modules)
        };
    }
}

#endif