#ifndef COPPER_COMPONENTS_RESPONSE_CORS_HPP
#define COPPER_COMPONENTS_RESPONSE_CORS_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/containers.hpp>
#include <copper/components/method.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response.hpp>

namespace copper::components {
/**
 * Generates response on CORS
 *
 * @param request
 * @param start_at
 * @param methods
 * @param state
 * @return response
 */
response response_cors(const request& request, long start_at,
                       const containers::vector_of<method>& methods);

}  // namespace copper::components

#endif