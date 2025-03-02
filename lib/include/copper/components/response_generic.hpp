#ifndef COPPER_COMPONENTS_RESPONSE_GENERIC_HPP
#define COPPER_COMPONENTS_RESPONSE_GENERIC_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/beast/http/message_generator.hpp>

namespace copper::components {

/**
 * Response generic type
 */
using response_generic = boost::beast::http::message_generator;

}  // namespace copper::components

#endif