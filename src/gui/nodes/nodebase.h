#pragma once
#include <vector>
#include <cinttypes>
#include <string>

namespace January::Engine::Node {
    struct InPin;
    struct OutPin;

    class NodeBase {
    public:
        NodeBase();
        virtual ~NodeBase();

        virtual void PreDraw(int32_t& uniqueId);
        virtual void Draw();
        virtual void PostDraw();

        int32_t get_pin_count_out();
        bool get_type_out(int32_t index, std::string& result);
        bool get_value_out(int32_t index, std::string& result);

    protected:
        int32_t get_pin_count_in();
        bool get_type_in(int32_t index, std::string& result);
        bool get_value_in(int32_t index, std::string& result);

        std::string title;
        std::vector<struct InPin*> inpins;
        std::vector<struct OutPin*> outpins;
    };
}