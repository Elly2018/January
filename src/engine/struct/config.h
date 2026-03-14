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
#ifndef ENGINE_STRUCT_CONFIG_H
#define ENGINE_STRUCT_CONFIG_H
#include <cinttypes>
#include <chrono>
#include <ctime>
#include <utility>
#include <string>
#include "nlohmann/json.hpp"

namespace sc = std::chrono;
using json = nlohmann::json;

namespace January::Engine {
    struct AppConfigRecent {
        std::string j_path                                       = "";
        time_t j_last_open                                       = sc::system_clock::to_time_t(sc::system_clock::now());
    };
    // Engine Configuration
    // Store information which can be modify and affect the inner workflow.
    // Such as FPS
    struct AppConfig {
        int32_t j_FPS                                            = 60;
        // Last open project
        std::string j_last_open                                  = "";
        std::vector<AppConfigRecent> j_recent                    = std::vector<AppConfigRecent>();
        std::vector<std::pair<int64_t, bool>> j_views_enable     = std::vector<std::pair<int64_t, bool>>();
        std::vector<std::string> j_workspace                     = std::vector<std::string>();
        std::vector<std::string> j_favorite                      = std::vector<std::string>();
        json j_project_setting                                   = json::object();
    };
}
#endif