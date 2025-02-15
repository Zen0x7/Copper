#include <copper/components/http_response_exception.hpp>

namespace copper::components {

    http_response http_response_exception(const http_request &request, long start_at) {
        const auto now = chronos::now();

        http_response response {http_status_code::internal_server_error, request.version()};
        response.set(http_fields::content_type, "application/json");

        response.set(http_fields::access_control_allow_headers, "Accept,Authorization,Content-Type,X-Requested-With");
        const auto allowed_origins = dotenv::getenv("HTTP_ALLOWED_ORIGINS", "*");
        response.set(http_fields::access_control_allow_origin, allowed_origins);

        response.set("X-Server", "Copper");
        response.set("X-Time", std::to_string(now - start_at));

        response.version(request.version());
        response.keep_alive(request.keep_alive());

        response.body() = std::string(R"({"message":"server_error"})");

        response.prepare_payload();

        return response;
    }
}