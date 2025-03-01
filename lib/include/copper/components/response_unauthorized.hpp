#ifndef COPPER_COMPONENTS_RESPONSE_UNAUTHORIZED_HPP
#define COPPER_COMPONENTS_RESPONSE_UNAUTHORIZED_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/request.hpp>
#include <copper/components/response.hpp>

namespace copper::components {
/**
 * Generate response on unauthorized
 *
 * @param request
 * @param start_at
 * @return response
 */
response response_unauthorized(const request& request, long start_at);

}  // namespace copper::components

#endif