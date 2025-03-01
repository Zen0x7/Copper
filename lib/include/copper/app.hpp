#ifndef COPPER_APP_HPP
#define COPPER_APP_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/configuration.hpp>
#include <string>

namespace copper {

/**
 * Get version
 *
 * @return string
 */
std::string get_version();

/**
 * Run
 *
 * @param argc
 * @param argv
 * @return int
 */
int run(int argc, const char* argv[]);

}  // namespace copper

#endif  // COPPER_APP_HPP
