#ifndef COPPER_COMPONENTS_RESPONSE_BAD_REQUEST_HPP
#define COPPER_COMPONENTS_RESPONSE_BAD_REQUEST_HPP

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
 * Generates response on bad requests
 *
 * @param request
 * @param start_at
 * @return response
 */
response response_bad_request(const request& request, long start_at);

}  // namespace copper::components

#endif