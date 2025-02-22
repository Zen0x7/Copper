#ifndef COPPER_COMPONENTS_HTTP_STATUS_CODE_HPP
#define COPPER_COMPONENTS_HTTP_STATUS_CODE_HPP

#pragma once

#include <boost/beast/http/status.hpp>

namespace copper::components {

/**
 * HTTP status codes
 */
typedef boost::beast::http::status http_status_code;

}  // namespace copper::components

#endif