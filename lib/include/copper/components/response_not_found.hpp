#ifndef COPPER_COMPONENTS_RESPONSE_NOT_FOUND_HPP
#define COPPER_COMPONENTS_RESPONSE_NOT_FOUND_HPP

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
 * Generate response on not found
 *
 * @param request
 * @param start_at
 * @param state
 * @return response
 */
response response_not_found(const request& request, long start_at);

}  // namespace copper::components

#endif