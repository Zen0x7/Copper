#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_TOO_MANY_REQUESTS_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_TOO_MANY_REQUESTS_HPP

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
 * Generate HTTP response on rate limit exceeded
 *
 * @param request
 * @param start_at
 * @param ttl
 * @param state
 * @return response
 */
response http_response_too_many_requests(const request& request,
                                         long start_at, int ttl,
                                         const shared<state>& state);

}  // namespace copper::components

#endif