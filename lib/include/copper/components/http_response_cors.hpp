#pragma once

#include <copper/components/http_response.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/http_method.hpp>
#include <copper/components/containers.hpp>

namespace copper::components {
    http_response http_response_cors(const http_request & request, long start_at, containers::vector_of<http_method> methods);
}