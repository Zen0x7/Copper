#ifndef COPPER_COMPONENTS_URL_HPP
#define COPPER_COMPONENTS_URL_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/request.hpp>

namespace copper::components {
/**
 * URL from request
 * @param request
 * @return string
 */
std::string url_from_request(const request& request);
}  // namespace copper::components

#endif