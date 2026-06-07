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
#include <atomic>
#include <filesystem>
#include <angelscript.h>
#include "../utility/mutex.h"

namespace January {
    namespace System {
        struct JWindow;
    }
    namespace Engine {
        struct JEngine;

        namespace fs = std::filesystem;

        namespace VM {
            void LogInfo(const std::string& msg);
            void LogWarning(const std::string& msg);
            void LogError(const std::string& msg);
        }

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

            /**
             * @brief Check current compile state
             * 
             * @return Is it compiling right now
             */
            bool IsCompiling();
            /**
             * @brief Running a single script in the asset browser
             * 
             * @param path The file path
             */
            void RunEditorScript(std::string path);
            /**
             * @brief When code is change or user want update VM.
             * This will rebuild the VM
             */
            void UpdateVMContent();

        protected:
            /**
             * @brief Register all the global variable and function and object into VM
             */
            void PrepareGlobal();
            /**
             * @brief Compile thread function, Put all the code into VM
             */
            void Compile();
            /**
             * @brief Given a file path, and read it and put it into the VM
             * 
             * @param file The path of that file
             */
            void CompileSingle(fs::path file);
            /**
             * @brief Deep search project assets file to get all the .as files
             * 
             * @return all the .as files path
             */
            std::vector<fs::path> GetAllScriptPath();
            /**
             * @brief The message feedback of the VM
             * 
             * @param msg VM message feedback object
             */
            void ScriptMessageCallback(const asSMessageInfo* msg);

        public:
            void LogInfo(const std::string& msg);
            void LogWarning(const std::string& msg);
            void LogError(const std::string& msg);

        protected:
            System::JWindow&    jwindow;
            JEngine&            jengine;

            /**
             * @brief Compiling state, Show if the VM is compiling right now or not
             */
            std::atomic_bool compiling = false;
            /**
             * @brief The VM handle
             */
            std::unique_ptr<asIScriptEngine, ASEngineDeleter> engine;
            /**
             * @brief The module, The string is the relative path to the file
             */
            std::vector<std::string> modules;
            j_mutex(modules)
        };
    }
}

#endif