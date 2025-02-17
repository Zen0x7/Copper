#include <copper/components/http_controller.hpp>

namespace copper::components {

    void http_controller::set_state(const shared<state> &state) { state_ = state; }

    void http_controller::set_bindings(containers::unordered_map_of_strings &bindings) {
        bindings_ = std::move(bindings);
    }

    // LCOV_EXCL_START
    void http_controller::set_data(const json::value &data) { data_ = data; }

    void http_controller::set_user(const uuid id) { auth_id_ = id; }

    void http_controller::set_start(long at) { start_ = at; }

    void http_controller::set_config(http_controller_config config) {
      config_ = config;
    }

    http_response
    http_controller::response(const http_request &request, const http_status_code status, const std::string &data,
                              const char *type) const {
      const auto resolved_at = chronos::now();

      http_response response{};

      response.set(http_fields::content_type, type);
      response.set(http_fields::allow, request.method_string());
      response.set(http_fields::access_control_allow_headers, "Accept,Authorization,Content-Type,X-Requested-With");

      const auto allowed_origins = dotenv::getenv("HTTP_ALLOWED_ORIGINS", "*");

      response.set(http_fields::access_control_allow_origin, allowed_origins);

      response.set("X-Server", "Copper");
      response.set("X-Time", std::to_string(resolved_at - start_));
      response.version(request.version());
      response.keep_alive(request.keep_alive());
      response.result(status);
      response.body() = data;
      response.prepare_payload();
      return response;
    }
    // LCOV_EXCL_STOP
}