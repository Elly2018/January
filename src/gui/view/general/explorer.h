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
    /**
     * @brief This content the file detail description
     */
    struct JFileContent {
        /**
         * @brief File unique uuid for engine
         */
        UUIDv4::UUID uuid;
        /**
         * @brief File title, normally should be filename or foldername without extension
         */
        std::string title;
        /**
         * @brief Absolute path for this file or folder
         */
        fs::path path;
        /**
         * @brief File or Folder
         */
        bool is_dir;
        /**
         * @brief The size of the file
         */
        uintmax_t filesize;
    };

    /**
     * @brief Folder tree node structure
     * This use for the left side of the panel
     */
    struct JFolderContent {
        /**
         * @brief Display label on the gui
         */
        std::string name;
        /**
         * @brief Relative path for this file or folder
         */
        std::string path;
        /**
         * @brief Is user open this folder or not
         */
        bool is_open;
        /**
         * @brief Children of the folder node
         */
        std::vector<JFolderContent*> children;
        /**
         * @brief Destroy all the children recursivily
         */
        void CleanChildren();
    };

    /**
     * @brief The asset browser view
     * Handle files and folder etc...
     * Should only show the project "Assets" relative folder content
     */
    class JViewExplorer : public JViewBase {
    public:
        DEFAULT_VIEW_CTOR(JViewExplorer) {}
        DEFAULT_VIEW_DECTOR(JViewExplorer) {}

        /**
         * @brief The right panel display mode currently
         */
        enum class DisplayMode {
            /**
             * @brief Show file in the disk
             */
            NORMAL = 0,
            /**
             * @brief Show search result
             */
            SEARCH = 1
        };

        /**
         * @brief The file type filter
         */
        enum class FilterFlag {
            /**
             * @brief The flag == this, this mean no filter at all
             */
            NONE = 0,
            PREFAB = 1 << 1,
            MATERIAL = 1 << 2,
            TEXTURE = 1 << 3,
        };

        /**
         * @brief The path bar input mode
         */
        enum class PathBarDisplay {
            /**
             * @brief Show the path buttons
             */
            DEFAULT = 0,
            /**
             * @brief Show a input field let user enter relative path
             */
            PATH_INPUT = 1,
            /**
             * @brief Show search input field
             */
            SEARCH_INPUT = 2,
        };

        /**
         * @brief Search state
         */
        enum class SearchState {
            /**
             * @brief Not start search yet
             */
            NONE = 0,
            /**
             * @brief Search background thread is running
             */
            SEARCHING = 1,
            /**
             * @brief It's done, scan finish
             */
            DONE = 2
        };

        void Init() override;
        void Update() override;
        void Draw() override;
        void DeInit() override;
        void Focus(bool value) override;

    protected:
        void DrawItemLine(JFileContent& target);
        void DrawItemGrid(JFileContent& target, int32_t size);
        /**
         * @brief Drawing path actions
         * Return last travel folder
         * 
         * @details It checks the path_input
         * 1: Go to parent folder
         * 2: Go to root folder, which is project path + "Assets"
         */
        void DrawPathAction();
        /**
         * @brief Drawing path bar, with a row of folder button for quick travel
         */
        void DrawPathBar();
        /**
         * @brief Left panel and right panel require splitter
         * And this function is that splitter
         * ImGui::SameLine is included, no need to write it
         */
        void DrawMiddleHandle();
        /**
         * @brief Render the folder tree
         * 
         * @param[in] tree Tree instance
         * @param[in] level Current depth level
         */
        void DrawLeftSideTreeNode(JFolderContent& tree, int32_t level);
        /**
         * @brief Drawing left panel content
         */
        void DrawLeftSide();
        /**
         * @brief Drawing right panel content
         */
        void DrawRightSide();
        /**
         * @brief Handle item tooltip event
         * 
         * @param _path The item's path
         * @param is_dir Is item directory or file
         * @param filesize The size of the file
         */
        void DrawItemTooltip(fs::path _path, bool is_dir = true, uintmax_t filesize = 0);
        /**
         * @brief Handle item event, such as right click or double click
         * 
         * @param _path The item's path
         * @param is_dir Is item directory or file
         * @param tree_node Is it called from tree render method
         */
        void DrawItemEvent(fs::path _path, bool is_dir = true, bool tree_node = false);
        /**
         * @brief Handle right panel background context menu event
         */
        void DrawRightSide_Event();
        /**
         * @brief Update the folder list, this should get called when "path" variable changed
         */
        void UpdatePathNode();
        /**
         * @brief Fire a thread to do a folder deep searching
         */
        void StartSearch();
        /**
         * @brief Base on Assets folder content
         * Create metadata for it
         */
        void StartEncoding();
        /**
         * @brief Apply the filter to this function
         * Return:
         * True: It passed, render it
         * False: Nope, ignore it
         */
        bool FilterCheck(JFileContent& file);
        /**
         * @brief Help with file watcher update
         * Monitor the "changed" value to switch focus folder
         */
        void UpdateFileWatcher();
        bool CheckSelection(JFileContent& target);
    public:
        /**
         * @brief Reset it
         * Should be called when load a project
         */
        void ReloadProject();
        /**
         * @brief You will get project + Assets
         * 
         * @return Get current asset folder (Absolute)
         */
        fs::path CurrentFolder();
        /**
         * @brief You will get project + .january + Assets
         * 
         * @return Get current meta asset folder (Absolute)
         */
        fs::path CurrentMetaFolder();
        /**
         * @brief The relative path base on project root folder
         */
        std::string path = "Assets";
        /**
         * @brief For the inputText, it need a buffer value
         * We can't just put "path" variable into there
         * We will have no idea to get the previous path to record
         */
        std::string path_buffer = "";
        /**
         * @brief Use for render path bar (The top bar with list of folder button for quick traval)
         */
        std::vector<std::string> path_node;

    private:
        /**
         * @brief The right panel content display mode
         */
        std::atomic<DisplayMode> mode = DisplayMode::NORMAL;
        /**
         * @brief The filter content flag
         */
        std::atomic<FilterFlag> filter = FilterFlag::NONE;
        /**
         * @brief Affect the button and other UI display
         */
        std::atomic<SearchState> search_state = SearchState::NONE;
        /**
         * @brief Search text
         */
        std::string search = "";
        /**
         * @brief Modify by tyhe file watcher worker callback
         * Does user change the path by input or enter folder etc...
         * When changed is true
         * It will re-generate the "files" variables
         */
        std::atomic_bool changed = false;
        /**
         * @brief The path bar display mode
         */
        std::atomic<PathBarDisplay> path_input = PathBarDisplay::DEFAULT;
        /**
         * @brief Init will be the trigger
         * Define if the first event call is on.
         */
        std::atomic_bool init = false;
        /**
         * @brief Because left right panel is using splitter
         * The left width is dynamic which required a variable to record it
         */
        std::atomic<float> leftWidth = 0;
        /**
         * @brief The slider
         * Range [0 - 10]
         * Display mode will change base on variable is 0 or not
         * 0: Line text display
         * 1-10: Grid item display
         */
        std::atomic_int32_t imgSize = 1;
        /**
         * @brief Folder watcher extension worker
         */
        filewatch::FileWatch<std::string>* watcher = nullptr;
        /**
         * @brief Right side files data
         */
        std::vector<JFileContent> files = std::vector<JFileContent>();
        std::mutex files_mtx;
        /**
         * @brief The left side folder tree structure
         */
        JFolderContent folder_node = JFolderContent();
        std::mutex folder_node_mtx;
        /**
         * @brief Remember the travel history
         * In order to make return last history button works
         */
        std::stack<std::string> travel_record;
        std::mutex travel_record_mtx;
    };
}
#endif