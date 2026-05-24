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
#ifndef ENGINE_UTILITY_COMMAND_H
#define ENGINE_UTILITY_COMMAND_H
#include <string>

namespace January {
    namespace System {
        struct JSystem;
    }
    namespace Engine {
        struct AppContext;
        void ApplyCommand(struct System::JSystem& jsystem, std::string cmd);
        // Single Command:
        // * config_dirty
        // * new_project
        // * open_project
        // * save_project
        // * save_project_as
        // * open_setting
        // * open_preference
        // Multi Command:
        // * open_recent <path>
        // * create_folder <path>
        // * create_resource <path>
        // * create_script <path>
        void PushCommand(struct AppContext& config, std::string cmd);
    }
}
#endif