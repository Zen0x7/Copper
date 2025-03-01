#ifndef COPPER_COMPONENTS_STATUS_CODE_HPP
#define COPPER_COMPONENTS_STATUS_CODE_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/beast/http/status.hpp>

namespace copper::components {

/**
 * Status codes
 */
typedef boost::beast::http::status status_code;

}  // namespace copper::components

#endif