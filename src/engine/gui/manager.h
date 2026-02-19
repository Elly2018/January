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
#include "blueprint.h"
#include "explorer.h"

namespace January::Engine::View {

    enum class JanuaryViewFlag {
        NONE = 0,
        EXPLORER = 0 << 1,
        BLUEPRINT = 0 << 1,
    };

    struct ViewManager {
        JViewExplorer       explorer;
        JViewBlueprint      blueprint;
    };

    void VInit(ViewManager& vm);
    void VDeInit(ViewManager& vm);
    void VSetEnable(ViewManager& vm, JanuaryViewFlag flag);
    // Drawing imgui context on screen
    void VDraw(JWindow& win, JEngine& engine);
    // Update Imgui context
    void VUpdate(JWindow& win, JEngine& engine);
}