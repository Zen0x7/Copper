#pragma once

#include <boost/beast.hpp>

namespace copper::components {
    typedef boost::beast::http::request<boost::beast::http::string_body> http_request;

    bool http_request_is_illegal(const http_request & request);
}