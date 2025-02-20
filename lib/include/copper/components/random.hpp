#ifndef COPPER_COMPONENTS_RANDOM_HPP
#define COPPER_COMPONENTS_RANDOM_HPP

#pragma once

#include <string>

namespace copper::components {

/**
 * Generate a random string
 *
 * @param size
 * @return
 */
std::string random_string(int size = 16);

}  // namespace copper::components

#endif
