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
#ifndef GUI_VIEW_VIEWBASE_H
#define GUI_VIEW_VIEWBASE_H
#include <cinttypes>
#include <string>

#define DEFAULT_VIEW_CTOR(x) \
x (std::string _title, int32_t _type, int32_t _subtype, System::JWindow& _win, JEngine& _engine) :  \
JViewBase(_title, _type, _subtype, _win, _engine) \

#define DEFAULT_VIEW_DECTOR(x) \
    virtual ~x() \

namespace January {
    namespace System {
        struct JWindow;
    }
    namespace Engine{
        struct JEngine;
        namespace View {
            // The base class for editor view
            class JViewBase {
            public:
                JViewBase(std::string _title, int32_t _type, int32_t _subtype, System::JWindow& _win, JEngine& _engine);
                virtual ~JViewBase();
                virtual void OnEnable() {};
                virtual void OnDisable() {};
                virtual void Init() {}
                virtual void DeInit() {}
                virtual void Focus(bool value) {}
                virtual void Update() {}
                virtual bool PreDraw();
                virtual void Draw() {}
                virtual void PostDraw();

                bool IsEnable() { return enable; }
                int64_t GetID();
                void SetEnable(bool value);
            public:
                std::string         title           = "";
                int32_t             type            = 0;
                int32_t             subtype         = 0;
            protected:
                System::JWindow&    jwindow;
                JEngine&            jengine;
                bool                enable          = false;
                int32_t             window_flag     = 0;
            };
        }
    }
}
#endif
