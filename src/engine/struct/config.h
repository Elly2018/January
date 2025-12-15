#pragma once
#include <cinttypes>
#include "context.h"

struct AppConfig {
    int32_t j_FPS           = 60;
    JPageType j_last_open   = JPageType::JPAGETYPE_RESOURCE;
    std::string j_page_name = "";
};