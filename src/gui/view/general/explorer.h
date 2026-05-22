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
#ifndef GUI_VIEW_EXPLORER_H
#define GUI_VIEW_EXPLORER_H
#include <vector>
#include <stack>
#include <string>
#include <mutex>
#include <filesystem>
#include <uuid_v4.h>
#include <FileWatch.hpp>
#include "../viewbase.h"

namespace fs = std::filesystem;

namespace January::Engine::View {
    //
    // This content the file detail description
    //
    struct JFileContent {
        // File unique uuid for engine
        UUIDv4::UUID uuid;
        // File title, normally should be filename or foldername without extension
        std::string title;
        // Absolute path for this file or folder
        fs::path path;
        // File or Folder
        bool is_dir;
        // The size of the file
        uintmax_t filesize;
    };

    //
    // Folder tree node structure
    // This use for the left side of the panel
    //
    struct JFolderContent {
        // Display label on the gui
        std::string name;
        // Is user open this folder or not
        bool is_open;
        // Children of the folder node
        std::vector<JFolderContent*> children;
        // Destroy all the children recursivily
        void CleanChildren();
    };

    //
    // The asset browser view
    // Handle files and folder etc...
    // Should only show the project "Assets" relative folder content
    //
    class JViewExplorer : public JViewBase {
    public:
        DEFAULT_VIEW_CTOR(JViewExplorer) {}
        DEFAULT_VIEW_DECTOR(JViewExplorer) {}
        void Init() override;
        void Update() override;
        void Draw() override;
        void DeInit() override;
        void Focus(bool value) override;

    protected:
        void DrawPathAction();
        void DrawPathBar();
        void DrawMiddleHandle();
        void DrawLeftSide();
        void DrawRightSide();
        void DrawItemTooltip(JFileContent& target);
        void DrawItemEvent(JFileContent& target);
        void DrawRightSide_Event();

        void UpdatePathNode();

    public:
        void ReloadProject();
        // You will get project + Assets
        fs::path CurrentFolder();
        // The relative path base on project "Assets" folder
        std::string path = "";
        std::vector<std::string> path_node;

    private:
        // Modify by tyhe file watcher worker callback
        // Does user change the path by input or enter folder etc...
        // When changed is true
        // It will re-generate the "files" variables
        bool changed = false;
        // Init will be the trigger
        // Define if the first event call is on.
        bool init = false;
        float leftWidth = 0;
        // The slider
        // Range [0 - 10]
        // Display mode will change base on variable is 0 or not
        // 0: Line text display
        // 1-10: Grid item display
        int32_t imgSize = 1;
        int32_t selection = 0;
        filewatch::FileWatch<std::string>* watcher = nullptr;
        std::mutex mtx;
        std::vector<JFileContent> files = std::vector<JFileContent>();
        JFolderContent Assets = JFolderContent();
        std::stack<std::string> travel_record;
    };
}
#endif