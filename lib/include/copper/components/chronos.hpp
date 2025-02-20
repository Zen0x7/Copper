#ifndef COPPER_COMPONENTS_CHRONOS_HPP
#define COPPER_COMPONENTS_CHRONOS_HPP

#pragma once

#include <chrono>

namespace copper::components::chronos {

long now();

long to_timestamp(std::chrono::system_clock::time_point clock);

}  // namespace copper::components::chronos

#endif