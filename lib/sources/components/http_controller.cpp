#include <copper/components/configuration.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/state.hpp>

namespace copper::components {

void http_controller::set_state(const shared<state> &state) { state_ = state; }

void http_controller::set_bindings(
    containers::unordered_map_of_strings &bindings) {
  bindings_ = std::move(bindings);
}

// LCOV_EXCL_START
void http_controller::set_body(const json::value &body) { body_ = body; }

void http_controller::set_user(const uuid id) { auth_id_ = id; }

void http_controller::set_start_at(long start_at) { start_at_ = start_at; }

void http_controller::set_configuration(
    http_controller_configuration configuration) {
  configuration_ = configuration;
}

http_response http_controller::make_response(const http_request &request,
                                             http_status_code status,
                                             const std::string &data,
                                             const char *type) const {
  const auto resolved_at = chronos::now();

  http_response response{};

  response.set(http_fields::content_type, type);
  response.set(http_fields::allow, request.method_string());
  const std::string allowed_headers =
      "Accept,Authorization,Content-Type,X-Requested-With";

  response.set(http_fields::access_control_allow_headers, allowed_headers);
  const auto allowed_origins =
      state_->get_configuration()->get()->http_allowed_origins_;

  response.set(http_fields::access_control_allow_origin, allowed_origins);

  response.set("X-Server", "Copper");
  response.set("X-Time", std::to_string(resolved_at - start_at_));
  response.version(request.version());
  response.keep_alive(request.keep_alive());
  response.result(status);
  response.body() = data;
  response.prepare_payload();
  return response;
}
// LCOV_EXCL_STOP
}  // namespace copper::components