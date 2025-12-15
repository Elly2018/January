#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../gui/view/viewbase.h"

enum JPageType {
    JPAGETYPE_CUSTOM = 0,
    JPAGETYPE_RESOURCE = 1,
    JPAGETYPE_COMPOSITING = 2,
    JPAGETYPE_SCENE = 3,
    JPAGETYPE_MATERIAL = 4,
    JPAGETYPE_AUDIO = 5,
    JPAGETYPE_LOGIC = 6,
};

enum JViewType {
    JVIEWTYPE_CUSTOM = 0,
    // Audio 1 - 99
    JVIEWTYPE_VOLUMN = 1,
    JVIEWTYPE_AUDIOMIXER = 2,
    // 3D 100 - 199
    JVIEWTYPE_MATERIAL = 100,
    JVIEWTYPE_HIERARCHY = 101,
    // Logic 200 - 299
    JVIEWTYPE_BLUEPRINT =- 200,
    JVIEWTYPE_SCRIPT =- 201,
    JVIEWTYPE_CONSOLE =- 202,
    JVIEWTYPE_DEBUGGER =- 203,
    // Other >= 1000
    JVIEWTYPE_EXPLORER = 1000,
    JVIEWTYPE_PREVIEW = 1001,
    JVIEWTYPE_RENDER = 1002,
    JVIEWTYPE_TIMELINE = 1003,
    JVIEWTYPE_MATERIALPREVIEW = 1004,
    JVIEWTYPE_INSPECTOR = 1005,
};

// Current application context
// This data cannot be store in disk, This is the memory only data
struct AppContext {
    std::string project_path                      = "";
    // Current page type
    JPageType page_type                           = JPageType::JPAGETYPE_RESOURCE;
    // Current live view instance
    std::vector<std::shared_ptr<JViewBase>> views = std::vector<std::shared_ptr<JViewBase>>();
};

inline std::vector<JViewType> GetDefaultViewByPage(JPageType page) {
    std::vector<JViewType> r = std::vector<JViewType>();
    switch(page){
        default:
        case JPageType::JPAGETYPE_CUSTOM:
            r.push_back(JViewType::JVIEWTYPE_PREVIEW);
            return r;
        case JPageType::JPAGETYPE_RESOURCE:
            r.push_back(JViewType::JVIEWTYPE_EXPLORER);
            return r;
        case JPageType::JPAGETYPE_COMPOSITING:
            r.push_back(JViewType::JVIEWTYPE_BLUEPRINT);
            r.push_back(JViewType::JVIEWTYPE_CONSOLE);
            r.push_back(JViewType::JVIEWTYPE_RENDER);
            r.push_back(JViewType::JVIEWTYPE_TIMELINE);
            return r;
        case JPageType::JPAGETYPE_SCENE:
            r.push_back(JViewType::JVIEWTYPE_HIERARCHY);
            r.push_back(JViewType::JVIEWTYPE_INSPECTOR);
            r.push_back(JViewType::JVIEWTYPE_TIMELINE);
            r.push_back(JViewType::JVIEWTYPE_PREVIEW);
            r.push_back(JViewType::JVIEWTYPE_RENDER);
            return r;
        case JPageType::JPAGETYPE_MATERIAL:
            r.push_back(JViewType::JVIEWTYPE_EXPLORER);
            r.push_back(JViewType::JVIEWTYPE_CONSOLE);
            r.push_back(JViewType::JVIEWTYPE_BLUEPRINT);
            r.push_back(JViewType::JVIEWTYPE_MATERIALPREVIEW);
            return r;
        case JPageType::JPAGETYPE_AUDIO:
            r.push_back(JViewType::JVIEWTYPE_VOLUMN);
            r.push_back(JViewType::JVIEWTYPE_AUDIOMIXER);
            return r;
        case JPageType::JPAGETYPE_LOGIC:
            r.push_back(JViewType::JVIEWTYPE_SCRIPT);
            r.push_back(JViewType::JVIEWTYPE_CONSOLE);
            r.push_back(JViewType::JVIEWTYPE_DEBUGGER);
            return r;
    }
}

inline std::string GetPageTypeString(JPageType page){
    switch(page){
        default:
        case JPageType::JPAGETYPE_RESOURCE:
            return "Resource";
        case JPageType::JPAGETYPE_COMPOSITING:
            return "Compositing";
        case JPageType::JPAGETYPE_SCENE:
            return "Scene";
        case JPageType::JPAGETYPE_MATERIAL:
            return "Material";
        case JPageType::JPAGETYPE_AUDIO:
            return "Audio";
        case JPageType::JPAGETYPE_LOGIC:
            return "Logic";
    }
}