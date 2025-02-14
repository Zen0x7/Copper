#include <copper/components/http_response_cors.hpp>

namespace copper::components {

    http_response http_response_cors(const http_request &request, long start_at, const containers::vector_of<http_method> methods) {
        const auto now = chronos::now();

        http_response response {methods.empty() ? http_status_code::method_not_allowed : http_status_code::ok, request.version()};
        response.set(http_fields::content_type, "application/json");

        if (methods.empty()) {
            response.set(http_fields::access_control_allow_methods, "");
        } else {
            containers::vector_of<std::string> authorized_methods;
            for (auto & verb : methods) authorized_methods.push_back(to_string(verb));
            const auto methods_as_string = boost::join(authorized_methods, ",");
            response.set(http_fields::access_control_allow_methods, methods_as_string);
        }

        response.set(http_fields::access_control_allow_headers, "Accept,Authorization,Content-Type,X-Requested-With");
        const auto allowed_origins = dotenv::getenv("HTTP_ALLOWED_ORIGINS", "*");
        response.set(http_fields::access_control_allow_origin, allowed_origins);

        response.set("X-Server", "Copper");
        response.set("X-Time", std::to_string(now - start_at));

        response.version(request.version());
        response.keep_alive(request.keep_alive());

        response.content_length(0);

        return response;
    }
}