#ifndef COPPER_COMPONENTS_MIME_TYPE_HPP
#define COPPER_COMPONENTS_MIME_TYPE_HPP

#pragma once

#include <boost/beast/core/string.hpp>

namespace copper::components {

/**
 * Get MIME type of path
 *
 * @param path
 * @return std::string Output
 */
boost::beast::string_view mime_type(boost::beast::string_view path);

}  // namespace copper::components

#endif