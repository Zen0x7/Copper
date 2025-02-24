#ifndef COPPER_COMPONENTS_STATUS_CODE_HPP
#define COPPER_COMPONENTS_STATUS_CODE_HPP

#pragma once

#include <boost/beast/http/status.hpp>

namespace copper::components {

/**
 * Status codes
 */
typedef boost::beast::http::status status_code;

}  // namespace copper::components

#endif