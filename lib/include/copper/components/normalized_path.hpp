#ifndef COPPER_COMPONENTS_NORMALIZED_PATH_HPP
#define COPPER_COMPONENTS_NORMALIZED_PATH_HPP

#pragma once

#include <boost/beast/core/string.hpp>

namespace copper::components {

/**
 * Get normalized path
 *
 * @param base
 * @param path
 * @return std::string Output
 */
std::string normalized_path(boost::beast::string_view base,
                            boost::beast::string_view path);

}  // namespace copper::components

#endif