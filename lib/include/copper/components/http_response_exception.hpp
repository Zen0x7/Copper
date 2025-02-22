#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_EXCEPTION_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_EXCEPTION_HPP

#pragma once

#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>

namespace copper::components {

/**
 * Generates HTTP response on exception
 *
 * @param request
 * @param start_at
 * @return http_response
 */
http_response http_response_exception(const http_request& request,
                                      long start_at);

}  // namespace copper::components

#endif