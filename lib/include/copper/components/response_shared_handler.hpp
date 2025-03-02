#ifndef COPPER_COMPONENTS_SHARED_HANDLER_HPP
#define COPPER_COMPONENTS_SHARED_HANDLER_HPP

#pragma once

#include <copper/components/request.hpp>
#include <copper/components/response.hpp>

namespace copper::components {
/**
 * Response shared handler
 *
 * @param request
 * @param response
 * @param start_at
 * @param ttl
 * @return response
 */
response response_shared_handler(const request &request, response &response,
                                 long start_at, int ttl = -1);
}  // namespace copper::components

#endif