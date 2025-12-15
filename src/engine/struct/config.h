#pragma once
#include <cinttypes>

struct AppConfig {
    int32_t j_FPS           = 60;
    JPageType j_last_open   = JPageType::Resource;
    std::string j_page_name = "";
};