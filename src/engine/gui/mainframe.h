#pragma once
#include <concepts>
#include "../../system/window.h"
#include "../engine.h"
#include "view/viewbase.h"

template<class T, class U>
concept Derived = std::is_base_of<U, T>::value;

// Drawing imgui context on screen
void UIDraw(const JWindow& win, JEngine& engine);
// Update Imgui context
void UIUpdate(JWindow& win, JEngine& engine);

bool UIPageFirstTimeFire(JEngine& engine, JPageType page);

template<Derived<JViewBase> T>
bool UITryAdd(JEngine& engine, const char* title, JViewType view);

void UIGenerateViews(JEngine& engine, std::vector<JViewType> views);

void UILoadPageFromDisk(JEngine& engine, std::string name);