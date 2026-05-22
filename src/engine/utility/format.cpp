#include "format.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string_view>

namespace January::Engine {
    std::string format_bytes(uint64_t bytes, int32_t precision){
        static constexpr std::string_view units[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB" };
    
        // Handle the 0 bytes case cleanly
        if (bytes == 0) {
            return "0 B";
        }

        // Calculate which unit bucket it falls into
        // i = floor(log_1024(bytes))
        int32_t i = static_cast<int32_t>(std::floor(std::log2(bytes) / 10.0));
        
        // Bounds check to avoid wrapping past Exabytes (EB)
        if (i >= static_cast<int32_t>(std::size(units))) {
            i = static_cast<int32_t>(std::size(units)) - 1;
        }

        // Special case: No decimals for raw bytes (B)
        if (i == 0) {
            return std::to_string(bytes) + " B";
        }

        // Scale the bytes down to the target unit
        double scaled = static_cast<double>(bytes) / std::pow(1024.0, i);

        // Format with stringstream to manage decimal precision safely
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << scaled << " " << units[i];
        return ss.str();
    }
}