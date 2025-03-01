#ifndef COPPER_COMPONENTS_GUNZIP_HPP
#define COPPER_COMPONENTS_GUNZIP_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <string>

namespace copper::components {
/**
 * Compress
 *
 * @param input
 * @return string
 */
std::string gunzip_compress(const std::string& input);

/**
 * Decompress
 *
 * @param input
 * @return string
 */
std::string gunzip_decompress(const std::string& input);
}  // namespace copper::components

#endif