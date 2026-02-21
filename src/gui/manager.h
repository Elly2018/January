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
#include <cinttypes>
#include <vector>

namespace January {
    namespace System {
        struct JSystem;
        struct JWindow;
    }
    namespace Engine {
        struct JEngine;
        struct AppConfig;
    }
    namespace Engine::View {
        struct JViewBase;
        struct JPopupBase;

        struct JViewBlueprint;
        struct JViewConsole;
        struct JViewExplorer;
        struct JViewInspector;
        struct JViewPreview;
        struct JViewProfile;
        struct JViewScript;
        struct JViewTimeline;
        struct JViewVolumn;

        struct JPopupFileDialog;
        struct JPopupProjectDashboard;

        enum class JanuaryViewTypeFlag {
            NONE = 0,
            POPUP = 0 << 1,
            GENERAL = 0 << 2,
            AUDIO = 0 << 3,
            RENDER = 0 << 4,
        };

        enum class JanuaryViewGeneralFlag {
            NONE = 0,
            BLUEPRINT = 1,
            CONSOLE = 2,
            EXPLORER = 3,
            INSPECTOR = 4,
            PREVIEW = 5,
            PROFILER = 6,
            SCRIPT = 7,
            TIMELINE = 8,
            VOLUMN = 9,
        };

        enum class JanuaryViewGeneralPopupFlag {
            NONE = 0,
            FILE_DIALOG = 1,
            PROJECT_DASHBOARD = 2,
        };

        struct ViewManager {
            struct JViewBlueprint*          blueprint;
            struct JViewConsole*            console;
            struct JViewExplorer*           explorer;
            struct JViewInspector*          inspector;
            struct JViewPreview*            preview;
            struct JViewProfiler*            profiler;
            struct JViewScript*             script;
            struct JViewTimeline*           timeline;
            struct JViewVolumn*             volumn;

            struct JPopupFileDialog*        file_dialog;
            struct JPopupProjectDashboard*  project_dashboard;

            std::vector<struct JViewBase*>  views;
            std::vector<struct JPopupBase*>  popups;
        };

        // View manager initialization
        // This will create all the instance out
        void VInit(ViewManager& vm, struct System::JSystem& jsystem);
        // View manager release resource
        // Delete the instance
        void VDeInit(ViewManager& vm);
        // Set the view instance enable variable
        // By search its type and subtype
        void VSetEnable(ViewManager& vm, int32_t type, int32_t subtype, bool value);
        // Fetch window widget enable variables to config object
        void SaveEnableConfig(ViewManager& vm, struct AppConfig& config);
        // Apply the config data to window widget enable variable
        void ApplyEnableConfig(ViewManager& vm, struct AppConfig& config);
        // Drawing imgui context on each views
        void VDraw(ViewManager& vm, System::JWindow& jwindow, JEngine& jengine);
        // Update call for each views
        void VUpdate(ViewManager& vm);
    }
}
#endif
