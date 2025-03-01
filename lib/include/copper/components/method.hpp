#ifndef COPPER_COMPONENTS_METHOD_HPP
#define COPPER_COMPONENTS_METHOD_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/beast/http/verb.hpp>

namespace copper::components {

/**
 * Method
 */
typedef boost::beast::http::verb method;

}  // namespace copper::components

#endif