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
#ifndef GUI_NODES_NODEBASE_H
#define GUI_NODES_NODEBASE_H
#include <vector>
#include <cinttypes>
#include <string>

// Quick way to create view subclass constructor
#define DEFAULT_NODE_CTOR(x) \
x (NodeType _type, std::string _title, std::string _path) :  \
NodeBase(_type, _title, _path) \

// Quick way to create view subclass deconstructor
#define DEFAULT_NODE_DECTOR(x) \
    virtual ~x() \

namespace January::Engine::Node {
    struct InPin;
    struct OutPin;

    enum class NodeType {
        COMP, SCRIPT, SHADER
    };

    class NodeBase {
    public:
        NodeBase(NodeType _type, std::string _title, std::string _path);
        virtual ~NodeBase();

        virtual void PreDraw(int32_t& uniqueId);
        virtual void Draw();
        virtual void PostDraw();

        int32_t get_pin_count_out();
        bool get_type_out(int32_t index, std::string& result);
        bool get_value_out(int32_t index, std::string& result);

        NodeType GetNodeType() { return node_type; }
    protected:
        int32_t get_pin_count_in();
        bool get_type_in(int32_t index, std::string& result);
        bool get_value_in(int32_t index, std::string& result);

        std::string title;
        std::vector<std::string> paths;
        std::vector<struct InPin*> inpins;
        std::vector<struct OutPin*> outpins;
    
    private:
        NodeType node_type = NodeType::COMP;
    };

    class NodeManager {

    };
}
#endif