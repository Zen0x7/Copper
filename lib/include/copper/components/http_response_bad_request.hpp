#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_BAD_REQUEST_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_BAD_REQUEST_HPP

#pragma once

#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>

namespace copper::components {

http_response http_response_bad_request(const http_request& request,
                                        long start_at);

}  // namespace copper::components

#endif