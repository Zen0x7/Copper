#ifndef COPPER_COMPONENTS_HEADER_HPP
#define COPPER_COMPONENTS_HEADER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/json.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response.hpp>
#include <string>

namespace copper::components {

/**
 * Retrieves headers from request
 *
 * @param request
 * @return string
 */
std::string header_from_request(const request& request);

/**
 * Retrieves headers from response
 *
 * @param response
 * @return string
 */
std::string header_from_response(const response& response);

}  // namespace copper::components

#endif