#pragma once
#include <cinttypes>
#include <string>

namespace January::Engine::Node {
    class PinBase {
    public:
        PinBase();
        virtual ~PinBase();
        std::string type;
    };

    class InPin : public PinBase {

    };

    class OutPin : public PinBase {

    };
}