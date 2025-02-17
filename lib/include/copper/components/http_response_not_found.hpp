#pragma once

#include <copper/components/http_response.hpp>
#include <copper/components/http_request.hpp>

namespace copper::components {
    http_response http_response_not_found(const http_request & request, long start_at);
}