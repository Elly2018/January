#pragma once
#include <string>
#include <vector>
#include "../gui/view/viewbase.h"

enum class JPageType {
    Custom = 0,
    Resource = 1,
    Compositing = 2,
    Scene = 3,
    Material = 4,
    Audio = 5,
    Logic = 6,
};

enum class JViewType {
    Custom = 0,
    // Audio 1 - 99
    Volumn = 1,
    AudioMixer = 2,
    // 3D 100 - 199
    Material = 100,
    Hierarchy = 101,
    // Logic 200 - 299
    Blueprint =- 200,
    Script =- 201,
    Console =- 202,
    Debugger =- 203,
    // Other >= 1000
    Explorer = 1000,
    Preview = 1001,
    Render = 1002,
    Timeline = 1003,
    MaterialPreview = 1004,
    Inspector = 1005,
};

// Current application context
// This data cannot be store in disk, This is the memory only data
struct AppContext {
    std::string project_path                      = "";
    // Current page type
    JPageType page_type                           = JPageType::Resource;
    // Current live view instance
    std::vector<std::shared_ptr<JViewBase>> views = std::vector<std::shared_ptr<JViewBase>>();
};

inline std::vector<JViewType> GetDefaultViewByPage(JPageType page) {
    std::vector<JViewType> r = std::vector<JViewType>();
    switch(page){
        default:
        case JPageType::Custom:
            r.push_back(JViewType::Preview);
            return r;
        case JPageType::Resource:
            r.push_back(JViewType::Explorer);
            return r;
        case JPageType::Compositing:
            r.push_back(JViewType::Blueprint);
            r.push_back(JViewType::Console);
            r.push_back(JViewType::Render);
            r.push_back(JViewType::Timeline);
            return r;
        case JPageType::Scene:
            r.push_back(JViewType::Hierarchy);
            r.push_back(JViewType::Inspector);
            r.push_back(JViewType::Timeline);
            r.push_back(JViewType::Preview);
            r.push_back(JViewType::Render);
            return r;
        case JPageType::Material:
            r.push_back(JViewType::Explorer);
            r.push_back(JViewType::Console);
            r.push_back(JViewType::Blueprint);
            r.push_back(JViewType::MaterialPreview);
            return r;
        case JPageType::Audio:
            r.push_back(JViewType::Volumn);
            r.push_back(JViewType::AudioMixer);
            return r;
        case JPageType::Logic:
            r.push_back(JViewType::Script);
            r.push_back(JViewType::Console);
            r.push_back(JViewType::Debugger);
            return r;
    }
}

inline std::string GetPageTypeString(JPageType page){
    switch(page){
        default:
        case JPageType::Resource:
            return "Resource";
        case JPageType::Compositing:
            return "Compositing";
        case JPageType::Scene:
            return "Scene";
        case JPageType::Material:
            return "Material";
        case JPageType::Audio:
            return "Audio";
        case JPageType::Logic:
            return "Logic";
    }
}