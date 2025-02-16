#include <copper/components/chronos.hpp>

namespace copper::components::chronos {
    long now() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
    }

    long to_timestamp(const std::chrono::system_clock::time_point clock) {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(clock.time_since_epoch()).count();
    }
}