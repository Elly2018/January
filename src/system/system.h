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
#ifndef SYSTEM_SYSTEM_H
#define SYSTEM_SYSTEM_H
#include <memory>
#include <vector>

namespace January {
namespace Engine {
struct JEngine;
}
namespace System {
struct JWindow;
struct JWindowRender;
using JWR = std::unique_ptr<JWindowRender>;

// System context
// Contain all window objects
struct JSystem {
  JSystem();
  ~JSystem();
  // Editor window instance
  std::unique_ptr<JWindow> window;
  // Extra pure windows
  std::vector<JWR> windows;
  // Engine instance for backgroud calculation
  struct Engine::JEngine *engine;
};

// Main logic goes here
void UpdateLoop(JSystem &jsystem);
// System init
// This will create the editor context and return it
int32_t SInit(JSystem &jsystem);
// January system running entry point
void SRun(JSystem &jsystem);
} // namespace System
} // namespace January
#endif
