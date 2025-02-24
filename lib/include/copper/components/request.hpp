#ifndef COPPER_COMPONENTS_REQUEST_HPP
#define COPPER_COMPONENTS_REQUEST_HPP

#pragma once

#include <boost/beast/http/string_body.hpp>

namespace copper::components {

/**
 * Request
 */
typedef boost::beast::http::request<boost::beast::http::string_body> request;

/**
 * Request is illegal
 *
 * @param request
 * @return bool
 */
bool request_is_illegal(const request& request);

}  // namespace copper::components

#endif