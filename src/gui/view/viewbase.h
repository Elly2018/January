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
#include <memory>
#include <string>
#include "../../engine/engine.h"
#include "../../system/window.h"

namespace January::Engine::View {
    // The base class for editor view
    class JViewBase {
    public:
        JViewBase(const char* _title, int32_t _type, int32_t _subtype, JWindow _win, JEngine _engine) : 
            title(_title), type(_type), subtype(_subtype), jwindow(_win), jengine(_engine) {
                Init();
            }
        virtual ~JViewBase() {}
        virtual void OnEnable() {};
        virtual void OnDisable() {};
        virtual void Init() {}
        virtual void Update() {}
        virtual void Draw() {}
        virtual void DeInit() {}

        void SetEnable(bool value);
    public:
        const char*     title;
        int32_t         type;
        int32_t         subtype;
    protected:
        JWindow         jwindow;
        JEngine         jengine;
    private:
        bool            enable;
    };
}
