#ifndef COPPER_COMPONENTS_HTTP_REQUEST_HPP
#define COPPER_COMPONENTS_HTTP_REQUEST_HPP

#pragma once

#include <boost/beast/http/string_body.hpp>

namespace copper::components {

/**
 * HTTP request
 */
typedef boost::beast::http::request<boost::beast::http::string_body>
    http_request;

/**
 * HTTP request is illegal
 *
 * @param request
 * @return bool
 */
bool http_request_is_illegal(const http_request& request);

}  // namespace copper::components

#endif