#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_UNAUTHORIZED_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_UNAUTHORIZED_HPP

#pragma once

#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {

/**
 * Forward state
 */
class state;

/**
 * Generate HTTP response on unauthorized
 *
 * @param request
 * @param start_at
 * @param state
 * @return http_response
 */
http_response http_response_unauthorized(const http_request& request,
                                         long start_at,
                                         const shared<state>& state);

}  // namespace copper::components

#endif