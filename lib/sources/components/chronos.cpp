#include <copper/components/chronos.hpp>

namespace copper::components::chronos {
    long now() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count();
    }
}