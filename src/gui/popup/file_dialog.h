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
#ifndef GUI_POPUP_FILE_DIALOG_H
#define GUI_POPUP_FILE_DIALOG_H
#include <cinttypes>
#include <vector>
#include <string>
#include "popupbase.h"

namespace January::Engine::View {
    typedef void (*DialogResultFeedback)(bool cancel, std::vector<std::string> results);

    class JPopupFileDialog : public JPopupBase {
    public:
        DEFAULT_POPUP_CTOR(JPopupFileDialog) {}
        DEFAULT_POPUP_DECTOR(JPopupFileDialog) {}

        void OnDisable() override;

        enum class DialogType {
            SINGLE_FILE,
            MULTIPLE_FILE,
            SINGLE_DIR
        };

        void SetTitle(std::string _newtitle);
        void SetFilter(std::vector<std::pair<std::string, std::string>> _filters);
        void SetDialogType(DialogType _dialog_type);
        void RegisterOneTimeFeedback(DialogResultFeedback _feedback);

        void Draw() override;

    private:
        std::vector<std::pair<std::string, std::string>> filters = std::vector<std::pair<std::string, std::string>>();
        DialogType dialog_type;
        DialogResultFeedback feedback;
    };
}

#endif