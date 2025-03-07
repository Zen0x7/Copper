#ifndef COPPER_COMPONENTS_NORMALIZED_PATH_HPP
#define COPPER_COMPONENTS_NORMALIZED_PATH_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/beast/core/string.hpp>

namespace copper::components {

/**
 * Get normalized path
 *
 * @param base
 * @param path
 * @return string Output
 */
std::string normalized_path(boost::beast::string_view base,
                            boost::beast::string_view path);

}  // namespace copper::components

#endif