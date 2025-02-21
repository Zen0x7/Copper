#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_TOO_MANY_REQUESTS_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_TOO_MANY_REQUESTS_HPP

#pragma once

#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>

namespace copper::components {

http_response http_response_too_many_requests(const http_request& request,
                                              long start_at, int ttl);

}  // namespace copper::components

#endif