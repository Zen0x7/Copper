#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_NOT_FOUND_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_NOT_FOUND_HPP

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
 * Generate HTTP response on not found
 *
 * @param request
 * @param start_at
 * @param state
 * @return response
 */
response http_response_not_found(const request& request,
                                 long start_at,
                                 const shared<state>& state);

}  // namespace copper::components

#endif