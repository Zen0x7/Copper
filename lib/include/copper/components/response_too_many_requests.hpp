#ifndef COPPER_COMPONENTS_RESPONSE_TOO_MANY_REQUESTS_HPP
#define COPPER_COMPONENTS_RESPONSE_TOO_MANY_REQUESTS_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/request.hpp>
#include <copper/components/response.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * Forward state
 */
class state;

/**
 * Generate response on rate limit exceeded
 *
 * @param request
 * @param start_at
 * @param ttl
 * @param state
 * @return response
 */
response response_too_many_requests(const request& request, long start_at,
                                    int ttl);

}  // namespace copper::components

#endif