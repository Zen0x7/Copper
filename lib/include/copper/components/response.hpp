#ifndef COPPER_COMPONENTS_RESPONSE_HPP
#define COPPER_COMPONENTS_RESPONSE_HPP

#pragma once

#include <boost/beast/http/string_body.hpp>

namespace copper::components {

/**
 * Response
 */
typedef boost::beast::http::response<boost::beast::http::string_body>
    response;

}  // namespace copper::components

#endif