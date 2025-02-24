#ifndef COPPER_COMPONENTS_RESPONSE_EXCEPTION_HPP
#define COPPER_COMPONENTS_RESPONSE_EXCEPTION_HPP

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
 * Generates response on exception
 *
 * @param request
 * @param start_at
 * @param state
 * @return response
 */
response response_exception(const request& request, long start_at,
                            const shared<state>& state);

}  // namespace copper::components

#endif