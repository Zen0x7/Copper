#ifndef COPPER_COMPONENTS_RESPONSE_NOT_FOUND_HPP
#define COPPER_COMPONENTS_RESPONSE_NOT_FOUND_HPP

#pragma once

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
response response_not_found(const request& request, long start_at,
                            const shared<state>& state);

}  // namespace copper::components

#endif