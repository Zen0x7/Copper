#pragma once

#include <copper/components/http_request.hpp>
#include <copper/components/http_response.hpp>

namespace copper::components {
http_response http_response_unauthorized(const http_request& request,
                                         long start_at);
}