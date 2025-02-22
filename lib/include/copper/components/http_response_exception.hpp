#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_EXCEPTION_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_EXCEPTION_HPP

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
 * Generates HTTP response on exception
 *
 * @param request
 * @param start_at
 * @param state
 * @return http_response
 */
http_response http_response_exception(const http_request& request,
                                      long start_at,
                                      const shared<state>& state);

}  // namespace copper::components

#endif