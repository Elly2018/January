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
#include "viewbase.h"
#include "../../engine/engine.h"
#include "../../engine/struct/context.h"
#include "../../engine/utility/command.h"
#include <imgui.h>

namespace January::Engine::View {
JViewBase::JViewBase(std::string _title, int32_t _type, int32_t _subtype,
                     System::JWindow &_win, JEngine &_engine)
    : title(_title), type(_type), subtype(_subtype), jwindow(_win),
      jengine(_engine) {}
JViewBase::~JViewBase() {}

bool JViewBase::PreDraw() {
  return ImGui::Begin(title.c_str(), &enable, window_flag);
}

void JViewBase::PostDraw() {
  ImGui::End();
  if (!enable) {
    PushCommand(*jengine.context, "config_dirty");
  }
}

int64_t JViewBase::GetID() {
  int64_t p = 0;
  int32_t *pt = (int32_t *)(&p);
  pt[0] = type;
  pt[1] = subtype;
  return p;
}

void JViewBase::SetEnable(bool value) {
  bool diff = value != enable;
  if (diff) {
    enable = value;
    if (enable)
      OnEnable();
    else
      OnDisable();
  }
}
} // namespace January::Engine::View
