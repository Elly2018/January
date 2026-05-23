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
        // Relative path for this file or folder
        std::string path;
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

        enum class DisplayMode {
            NORMAL = 0,
            SEARCH = 1
        };

        enum class FilterFlag {
            NONE = 0,
            PREFAB = 1 << 1,
            MATERIAL = 1 << 2,
            TEXTURE = 1 << 3,
        };

        enum class PathBarDisplay {
            DEFAULT = 0,
            PATH_INPUT = 1,
            SEARCH_INPUT = 2,
        };

        void Init() override;
        void Update() override;
        void Draw() override;
        void DeInit() override;
        void Focus(bool value) override;

    protected:
        void DrawItemLine(JFileContent& target);
        void DrawItemGrid(JFileContent& target, int32_t size);
        // Drawing path actions
        // 0: Return last travel folder
        // 1: Go to parent folder
        // 2: Go to root folder, which is project path + "Assets"
        void DrawPathAction();
        // Drawing path bar, with a row of folder button for quick travel
        void DrawPathBar();
        // Left panel and right panel require splitter
        // And this function is that splitter
        // ImGui::SameLine is included, no need to write it
        void DrawMiddleHandle();
        // Render the folder tree
        void DrawLeftSideTreeNode(JFolderContent& tree, int32_t level);
        // Drawing left panel content
        void DrawLeftSide();
        // Drawing right panel content
        void DrawRightSide();
        // Handle item tooltip event
        void DrawItemTooltip(fs::path _path, bool is_dir = true, uintmax_t filesize = 0);
        // Handle item event, such as right click or double click
        void DrawItemEvent(fs::path _path, bool is_dir = true, bool tree_node = false);
        // Handle right panel background context menu event
        void DrawRightSide_Event();
        // Update the folder list, this should get called when "path" variable changed
        void UpdatePathNode();
        // Fire a thread to do a folder deep searching
        void StartSearch();
        // Base on Assets folder content
        // Create metadata for it
        void StartEncoding();
        // Apply the filter to this function
        // Return:
        // True: It passed, render it
        // False: Nope, ignore it
        bool FilterCheck(JFileContent& file);
        // Help with file watcher update
        // Monitor the "changed" value to switch focus folder
        void UpdateFileWatcher();
    public:
        // Reset it
        // Should be called when load a project
        void ReloadProject();
        // You will get project + Assets
        fs::path CurrentFolder();
        // You will get project + .january + Assets
        fs::path CurrentMetaFolder();
        // The relative path base on project root folder
        std::string path = "Assets";
        // For the inputText, it need a buffer value
        // We can't just put "path" variable into there
        // We will have no idea to get the previous path to record
        std::string path_buffer = "";
        // Use for render path bar (The top bar with list of folder button for quick traval)
        std::vector<std::string> path_node;

    private:
        // The right panel content display mode
        std::atomic<DisplayMode> mode = DisplayMode::NORMAL;
        // The filter content flag
        std::atomic<FilterFlag> filter = FilterFlag::NONE;
        // Search text
        std::string search = "";
        // Modify by tyhe file watcher worker callback
        // Does user change the path by input or enter folder etc...
        // When changed is true
        // It will re-generate the "files" variables
        std::atomic_bool changed = false;
        // The path bar display mode
        std::atomic<PathBarDisplay> path_input = PathBarDisplay::DEFAULT;
        // Init will be the trigger
        // Define if the first event call is on.
        std::atomic_bool init = false;
        // Because left right panel is using splitter
        // The left width is dynamic which required a variable to record it
        std::atomic<float> leftWidth = 0;
        // The slider
        // Range [0 - 10]
        // Display mode will change base on variable is 0 or not
        // 0: Line text display
        // 1-10: Grid item display
        std::atomic_int32_t imgSize = 1;
        // Folder watcher extension worker
        filewatch::FileWatch<std::string>* watcher = nullptr;
        // Right side files data
        std::vector<JFileContent> files = std::vector<JFileContent>();
        std::mutex files_mtx;
        // The left side folder tree structure
        JFolderContent folder_node = JFolderContent();
        std::mutex folder_node_mtx;
        // Remember the travel history
        // In order to make return last history button works
        std::stack<std::string> travel_record;
        std::mutex travel_record_mtx;
    };
}
#endif