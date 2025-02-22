#ifndef COPPER_COMPONENTS_RANDOM_HPP
#define COPPER_COMPONENTS_RANDOM_HPP

#pragma once

#include <string>

namespace copper::components {

/**
 * Get random string
 *
 * @param size
 * @return string
 */
std::string random_string(int size = 16);

}  // namespace copper::components

#endif
