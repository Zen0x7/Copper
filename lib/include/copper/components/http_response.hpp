#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_HPP

#pragma once

#include <boost/beast/http/string_body.hpp>

namespace copper::components {

typedef boost::beast::http::response<boost::beast::http::string_body>
    http_response;

}  // namespace copper::components

#endif