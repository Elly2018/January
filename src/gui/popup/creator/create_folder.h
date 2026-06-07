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
#ifndef GUI_POPUP_CREATOR_CREATE_FOLDER_H
#define GUI_POPUP_CREATOR_CREATE_FOLDER_H
#include "../popupbase.h"
#include <atomic>
#include <mutex>
#include <string>

namespace January::Engine::View {
    class JPopupCreateFolder : public JPopupBase {
    public:
        DEFAULT_POPUP_CTOR(JPopupCreateFolder) {}
        DEFAULT_POPUP_DECTOR(JPopupCreateFolder) {}

        void Init() override;
        void Update() override;
        void Draw() override;

    public:
        void RegisterFolder(std::string _folder);

    private:
        std::string folder;
        std::string input;
        std::mutex input_mtx;
        std::string message;
        std::mutex message_mtx;
        std::atomic_bool can_be_confirm;
        std::atomic_bool trigger_create;
    };
}
#endif