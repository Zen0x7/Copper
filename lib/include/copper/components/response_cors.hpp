#ifndef COPPER_COMPONENTS_RESPONSE_CORS_HPP
#define COPPER_COMPONENTS_RESPONSE_CORS_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response.hpp>
#include <copper/components/method.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
/**
 * Forward state
 */
class state;

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
                       containers::vector_of<method> methods,
                       const shared<state>& state);

}  // namespace copper::components

#endif