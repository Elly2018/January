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
#include <memory>

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

        struct JViewApplication;
        struct JViewBlueprint;
        struct JViewConsole;
        struct JViewExplorer;
        struct JViewHierarchy;
        struct JViewInspector;
        struct JViewPreview;
        struct JViewProfiler;
        struct JViewProfile;
        struct JViewScript;
        struct JViewTimeline;
        struct JViewVolumn;

        struct JPopupCreateFolder;
        struct JPopupCreateResource;
        struct JPopupCreateScript;
        struct JPopupProjectDashboard;

        enum class JanuaryViewTypeFlag {
            NONE = 0,
            POPUP = 1 << 0,
            GENERAL = 1 << 1,
            AUDIO = 1 << 2,
            RENDER = 1 << 3,
            SCRIPT = 1 << 4,
            DEBUG = 1 << 5,
            ANIMATION = 1 << 6,
        };

        enum class JanuaryViewGeneralFlag {
            NONE = 0,
            APPLICATION = 1,
            BLUEPRINT = 2,
            CONSOLE = 3,
            EXPLORER = 4,
            HIERARCHY = 5,
            INSPECTOR = 6,
        };

        enum class JanuaryViewAudioFlag {
            NONE = 0,
            VOLUMN = 1,
            MIXER = 2,
            EDITOR = 3,
        };

        enum class JanuaryViewRenderFlag {
            NONE = 0,
            PREVIEW = 1,
            MATERIAL = 2,
        };

        enum class JanuaryViewScriptFlag {
            NONE = 0,
            SCRIPT = 1,
            Debug = 2,
        };

        enum class JanuaryViewDebugFlag {
            NONE = 0,
            PROFILER = 1,
        };

        enum class JanuaryViewAnimationFlag {
            NONE = 0,
            TIMELINE = 1
        };

        enum class JanuaryViewGeneralPopupFlag {
            NONE = 0,
            PROJECT_DASHBOARD = 1,
            PREFERENCE = 2,
            SETTING = 3,
            CREATE_FOLDER = 10,
            CREATE_RESOURCE = 11,
            CREATE_SCRIPT = 12,
        };

        /**
         * @brief A list of the view and popup object
         */
        struct ViewManager {
            std::shared_ptr<JViewApplication>        application;
            std::shared_ptr<JViewBlueprint>          blueprint;
            std::shared_ptr<JViewConsole>            console;
            std::shared_ptr<JViewExplorer>           explorer;
            std::shared_ptr<JViewHierarchy>          hierarchy;
            std::shared_ptr<JViewInspector>          inspector;
            std::shared_ptr<JViewPreview>            preview;
            std::shared_ptr<JViewProfiler>           profiler;
            std::shared_ptr<JViewScript>             script;
            std::shared_ptr<JViewTimeline>           timeline;
            std::shared_ptr<JViewVolumn>             volumn;

            std::shared_ptr<JPopupCreateFolder>      create_folder;
            std::shared_ptr<JPopupCreateResource>    create_resource;
            std::shared_ptr<JPopupCreateScript>      create_script;
            std::shared_ptr<JPopupProjectDashboard>  project_dashboard;

            std::vector<std::shared_ptr<JViewBase>>  views;
            std::vector<std::shared_ptr<JPopupBase>> popups;
            std::vector<int32_t> popup_orders;
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
        // Render Popuo
        void VRenderPopup(ViewManager& vm, std::vector<int32_t>& arrays, int32_t index);
        void FocusEvent(ViewManager& vm, bool v);
    }
}
#endif
