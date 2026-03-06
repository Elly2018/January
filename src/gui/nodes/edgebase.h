#pragma once
#include <cinttypes>

namespace January::Engine::Node {
    class EdgeBase {
    public:
        EdgeBase();
        virtual ~EdgeBase();

        int32_t id0;
        int32_t id1;
        int32_t slot0;
        int32_t slot1;
    };
}