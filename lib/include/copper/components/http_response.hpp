#pragma once

#include <boost/beast.hpp>

namespace copper::components {
    typedef boost::beast::http::response<boost::beast::http::string_body> http_response;
}