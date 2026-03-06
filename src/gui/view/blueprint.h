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
#ifndef GUI_VIEW_BLUEPRINT_H
#define GUI_VIEW_BLUEPRINT_H
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
        struct BlueprintObject {
            bool dirty;
            std::string file;
            std::string path;
            struct ax::NodeEditor::EditorContext* ctx;
            std::vector<struct Node::NodeBase*> nodes;
            std::vector<struct Node::EdgeBase*> edges;
        };

        class JViewBlueprint : public JViewBase {
        public:
            DEFAULT_VIEW_CTOR(JViewBlueprint) {}
            DEFAULT_VIEW_DECTOR(JViewBlueprint) {}
            void Init() override;
            void Update() override;
            void Draw() override;
            void DeInit() override;
        protected:
            virtual void DrawBlueprint(BlueprintObject& bo);
            virtual void CreateTempConfig();
        private:
            std::vector<BlueprintObject> blueprints;
        };
    }
}
#endif