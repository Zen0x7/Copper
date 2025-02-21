#ifndef COPPER_COMPONENTS_HTTP_RESPONSE_CORS_HPP
#define COPPER_COMPONENTS_HTTP_RESPONSE_CORS_HPP

#pragma once

#include <copper/components/containers.hpp>
#include <copper/components/http_method.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>

namespace copper::components {

http_response http_response_cors(const http_request& request, long start_at,
                                 containers::vector_of<http_method> methods);

}  // namespace copper::components

#endif