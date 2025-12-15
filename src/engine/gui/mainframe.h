#pragma once
#include "../../system/window.h"
#include "../engine.h"

// Drawing imgui context on screen
void UIDraw(JWindow& win, JEngine& engine);
// Update Imgui context
void UIUpdate(JWindow& win, JEngine& engine);

bool UIPageFirstTimeFire(JEngine& engine, JPageType page);

void UIGenerateViews(std::vector<JViewType> views);

void UILoadPageFromDisk(std::string name);