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
#include <vector>
#include <string>
#include <filesystem>
#include <uuid_v4.h>
#include <FileWatch.hpp>
#include <mutex>
#include "viewbase.h"

namespace fs = std::filesystem;

namespace January::Engine::View {
    struct JFileContent {
        UUIDv4::UUID uuid;
        std::string title;
        fs::path path;
        bool is_dir;
        uintmax_t filesize;
    };

    class JViewExplorer : public JViewBase {
    public:
        void Init() override;
        void Update() override;
        void Draw() override;
        void DeInit() override;

    protected:
        void DrawLeftSide();
        void DrawRightSide();

    public:
        std::string path = "";

    private:
        bool changed = false;
        bool init = false;
        float leftWidth = 0;
        float rightWidth = 0;
        float imgSize = 0.5f;
        int32_t selection = 0;
        filewatch::FileWatch<std::string>* watcher = nullptr;
        std::mutex mtx;
        std::vector<JFileContent> files = std::vector<JFileContent>();
    };
}