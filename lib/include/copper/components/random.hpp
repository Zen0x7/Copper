#ifndef COPPER_COMPONENTS_RANDOM_HPP
#define COPPER_COMPONENTS_RANDOM_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

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
