#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_CORS_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_CORS_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/http_method.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
/**
 * Forward state
 */
class state;

/**
 * Generates HTTP response on CORS
 *
 * @param request
 * @param start_at
 * @param methods
 * @param state
 * @return http_response
 */
http_response http_response_cors(const http_request& request, long start_at,
                                 containers::vector_of<http_method> methods,
                                 const shared<state>& state);

}  // namespace copper::components

#endif