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
#ifndef ENGINE_STRUCT_SETTING_H
#define ENGINE_STRUCT_SETTING_H
#include <cinttypes>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace January::Engine {
    //
    // The serialized data will be store in project/config.json
    //
    struct ProjectConfig {
        int32_t j_FPS                                            = 60;
        // Local project setting
        // If you're looking for format detail, go check config.h
        json j_project_setting                                   = json::object();
    };
}
#endif