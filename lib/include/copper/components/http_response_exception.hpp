#pragma once

#include <copper/components/http_response.hpp>
#include <copper/components/http_request.hpp>
#include <copper/components/http_method.hpp>
#include <copper/components/http_fields.hpp>
#include <copper/components/http_status_code.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/dotenv.hpp>

namespace copper::components {
    http_response http_response_exception(const http_request & request, long start_at);
}