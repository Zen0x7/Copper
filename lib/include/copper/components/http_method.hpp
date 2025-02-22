#ifndef COPPER_COMPONENTS_HTTP_METHOD_HPP
#define COPPER_COMPONENTS_HTTP_METHOD_HPP

#pragma once

#include <boost/beast/http/verb.hpp>

namespace copper::components {

/**
 * HTTP method
 */
typedef boost::beast::http::verb http_method;

}  // namespace copper::components

#endif