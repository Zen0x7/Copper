#ifndef COPPER_COMPONENTS_RESPONSE_HPP
#define COPPER_COMPONENTS_RESPONSE_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/beast/http/string_body.hpp>

namespace copper::components {

/**
 * Response
 */
typedef boost::beast::http::response<boost::beast::http::string_body> response;

}  // namespace copper::components

#endif