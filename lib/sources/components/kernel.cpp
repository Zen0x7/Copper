#include <boost/algorithm/string/predicate.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/cache.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/header.hpp>
#include <copper/components/http_response_bad_request.hpp>
#include <copper/components/http_response_cors.hpp>
#include <copper/components/http_response_exception.hpp>
#include <copper/components/http_response_not_found.hpp>
#include <copper/components/http_response_too_many_requests.hpp>
#include <copper/components/http_response_unauthorized.hpp>
#include <copper/components/kernel.hpp>
#include <copper/components/mime_type.hpp>
#include <copper/components/path.hpp>
#include <copper/components/query.hpp>
#include <copper/components/route_find.hpp>
#include <copper/components/route_match.hpp>
#include <copper/components/state.hpp>
#include <copper/components/validator.hpp>
#include <copper/models/request.hpp>
#include <copper/models/response.hpp>

namespace copper::components {

containers::optional_of<kernel_result> kernel::find_on_routes(
    const http_request &request) const {
  for (const auto &[route, controller] : *state_->get_router()->get_routes()) {
    if (auto [matches, bindings] =
            route_match(request.method(), request.target(), route);
        matches) {
      return kernel_result{
          .route_ = route, .controller_ = controller, .bindings_ = bindings};
    }
  }

  return boost::none;
}

containers::vector_of<method> kernel::get_available_methods(
    const http_request &request) const {
  containers::vector_of<method> methods;
  for (const auto &[route, controller] : *state_->get_router()->get_routes()) {
    if (auto [matches, bindings] = route_find(request.target(), route); matches)
      methods.push_back(route.method_);
  }
  return methods;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

containers::async_of<
    std::tuple<shared<copper::models::request>,
               shared<copper::models::response>, http_response_generic>>
kernel::call(uuid session_id, boost::beast::string_view,
             const http_request &request, const std::string &ip,
             const uuid &request_id, long now) const {
  auto _request =
      models::request_from_http_request(session_id, request_id, now, request);

  if (const auto route = find_on_routes(request); route.has_value()) {
    containers::unordered_map_of_strings bindings = route.value().bindings_;

    route.value().controller_->set_start_at(now);

    if (route.value().controller_->configuration_.use_throttler_) {
      if (auto [can, TTL] = co_await state_->get_cache()->can_invoke(
              request, ip, route.value().controller_->configuration_.rpm_);
          !can) {
        auto _http_response =
            http_response_too_many_requests(request, now, TTL, state_);

        auto _response = copper::models::response_from_http_response(
            session_id, _request, _http_response);

        co_return std::make_tuple(_request, _response, _http_response);
      }
    }

    if (route.value().controller_->configuration_.use_auth_) {
      std::string bearer{request["Authorization"]};

      std::string token =
          boost::starts_with(bearer, "Bearer ") ? bearer.substr(7) : bearer;

      boost::optional<authentication_result> user_id;
      user_id = authentication_from_bearer(
          bearer, state_->get_configuration()->get()->app_key_);

      if (!user_id.has_value()) {
        auto _http_response = http_response_unauthorized(request, now, state_);

        auto _response = copper::models::response_from_http_response(
            session_id, _request, _http_response);

        _response->protected_ =
            route.value().controller_->configuration_.use_protector_ == true;

        co_return std::make_tuple(_request, _response, _http_response);
      };

      route.value().controller_->set_user(user_id.get().id_);
    }

    route.value().controller_->set_bindings(bindings);
    route.value().controller_->set_state(state_);

    if (route.value().controller_->configuration_.use_validator_) {
      boost::system::error_code json_parse_error_code;

      json::value value =
          boost::json::parse(request.body(), json_parse_error_code);

      if (!json_parse_error_code) {
        route.value().controller_->set_body(value);

        auto rules = route.value().controller_->rules();

        if (auto validator = validator_make(rules, value);
            !validator->success_) {
          auto error_response =
              json::object({{"message", "The given data was invalid."},
                            {"errors", validator->errors_}});

          auto _http_response = route.value().controller_->make_response(
              request, status_code::unprocessable_entity,
              serialize(error_response), "application/json");

          auto _response = copper::models::response_from_http_response(
              session_id, _request, _http_response);

          _response->protected_ =
              route.value().controller_->configuration_.use_protector_ == true;

          co_return std::make_tuple(_request, _response, _http_response);
        }
      } else {
        auto error_response = json::object(
            {{"message", "The given data was invalid."},
             {"errors", {{"*", "The body must be a valid JSON."}}}});

        auto _http_response = route.value().controller_->make_response(
            request, status_code::unprocessable_entity,
            serialize(error_response), "application/json");

        auto _response = copper::models::response_from_http_response(
            session_id, _request, _http_response);

        _response->protected_ =
            route.value().controller_->configuration_.use_protector_ == true;

        co_return std::make_tuple(_request, _response, _http_response);
      }
    }

    try {
      auto _http_response = co_await route.value().controller_->invoke(request);

      auto _response = copper::models::response_from_http_response(
          session_id, _request, _http_response);

      _response->protected_ =
          route.value().controller_->configuration_.use_protector_ == true;

      co_return std::make_tuple(_request, _response, _http_response);
    } catch (std::exception &exception) {
      auto _http_response = http_response_exception(request, now, state_);

      auto _response = copper::models::response_from_http_response(
          session_id, _request, _http_response);

      _response->protected_ =
          route.value().controller_->configuration_.use_protector_ == true;

      co_return std::make_tuple(_request, _response, _http_response);
    }
  }

  if (request.method() == method::options) {
    auto available_verbs = get_available_methods(request);

    auto _http_response =
        http_response_cors(request, now, available_verbs, state_);

    auto _response = copper::models::response_from_http_response(
        session_id, _request, _http_response);

    co_return std::make_tuple(_request, _response, _http_response);
  }

  if (http_request_is_illegal(request)) {
    auto _http_response = http_response_bad_request(request, now, state_);

    auto _response = copper::models::response_from_http_response(
        session_id, _request, _http_response);

    co_return std::make_tuple(_request, _response, _http_response);
  }

  auto _http_response = http_response_not_found(request, now, state_);

  auto _response = copper::models::response_from_http_response(
      session_id, _request, _http_response);

  co_return std::make_tuple(_request, _response, _http_response);
}
}  // namespace copper::components