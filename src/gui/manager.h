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
#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

namespace January {
    namespace System {
        struct JSystem;
    }
    namespace Engine {
        struct JEngine;
    }
    namespace Engine::View {

        struct JViewExplorer;
        struct JViewBlueprint;

        enum class JanuaryViewTypeFlag {
            NONE = 0,
            GENERAL = 0 << 1,
            AUDIO = 0 << 2,
            RENDER = 0 << 3,
        };

        enum class JanuaryViewGeneralFlag {
            NONE = 0,
            EXPLORER = 0 << 1,
            BLUEPRINT = 0 << 2,
        };

        struct ViewManager {
            struct JViewExplorer       explorer;
            struct JViewBlueprint      blueprint;
        };

        void VInit(ViewManager& vm, System::JSystem& jsystem);
        void VDeInit(ViewManager& vm);
        void VSetEnable(ViewManager& vm, int32_t type, int32_t subtype);
        // Drawing imgui context on screen
        void VDraw(System::JWindow& win, Engine::JEngine& engine);
        // Update Imgui context
        void VUpdate(System::JWindow& win, Engine::JEngine& engine);
    }
}
#endif