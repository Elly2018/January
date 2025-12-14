#pragma once
#include <memory>
#include "struct/config.h"
#include "struct/context.h"

// Save app config to app preference location
void SaveAppConfig(std::weak_ptr<AppConfig> target);
// Load app config from app preference location
std::shared_ptr<AppConfig> LoadAppConfig();
// Generate a app context data struct
std::shared_ptr<AppContext> GenerateAppContext();

void EngineInit();
void EngineLoop();
void EngineDraw();