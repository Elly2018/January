#pragma once
#include <string>

enum JViewType {
    RESOURCE = 0
};

struct AppContext {
    std::string project_path = "";
    JViewType view_type      = JViewType::RESOURCE;
};