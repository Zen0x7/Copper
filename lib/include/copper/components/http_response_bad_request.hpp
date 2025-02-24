#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_BAD_REQUEST_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_BAD_REQUEST_HPP

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
 * Generates HTTP response on bad requests
 *
 * @param request
 * @param start_at
 * @param state
 * @return response
 */
response http_response_bad_request(const request& request,
                                   long start_at,
                                   const shared<state>& state);

}  // namespace copper::components

#endif