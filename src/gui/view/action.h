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
#ifndef GUI_VIEW_ACTION_H
#define GUI_VIEW_ACTION_H
#include "viewbase.h"
#include <vector>
#include <string>

namespace ax::NodeEditor {
    struct EditorContext;
}

namespace January::Engine {

    namespace Node {
        struct NodeBase;
        struct EdgeBase;
    }

    namespace View {

        class JViewAction : public JViewBase {
        public:
            DEFAULT_VIEW_CTOR(JViewAction) {}
            DEFAULT_VIEW_DECTOR(JViewAction) {}
            void Init() override;
            void Update() override;
            void Draw() override;
            void DeInit() override;
        protected:
            void Play();
            void Pause();
            void Stop();
        private:
            bool is_play;
            bool is_pause;
        };
    }
}
#endif