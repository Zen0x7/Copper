#pragma once

#include <boost/beast/http/string_body.hpp>

namespace copper::components {
typedef boost::beast::http::request<boost::beast::http::string_body>
    http_request;

bool http_request_is_illegal(const http_request& request);
}  // namespace copper::components