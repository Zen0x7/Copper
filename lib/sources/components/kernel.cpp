#include <boost/algorithm/string/predicate.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/cache.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/header.hpp>
#include <copper/components/kernel.hpp>
#include <copper/components/mime_type.hpp>
#include <copper/components/path.hpp>
#include <copper/components/query.hpp>
#include <copper/components/response_bad_request.hpp>
#include <copper/components/response_cors.hpp>
#include <copper/components/response_exception.hpp>
#include <copper/components/response_not_found.hpp>
#include <copper/components/response_too_many_requests.hpp>
#include <copper/components/response_unauthorized.hpp>
#include <copper/components/route_find.hpp>
#include <copper/components/route_match.hpp>
#include <copper/components/state.hpp>
#include <copper/components/validator.hpp>
#include <copper/components/url.hpp>
#include <copper/models/request.hpp>
#include <copper/models/response.hpp>

namespace copper::components {

containers::optional_of<kernel_result> kernel::find_on_routes(
    method method, const std::string &url) const {
  for (const auto &[route, controller] : *state_->get_router()->get_routes()) {
    if (auto [matches, bindings] = route_match(method, url, route); matches) {
      return kernel_result{
          .route_ = route, .controller_ = controller, .bindings_ = bindings};
    }
  }

  return boost::none;
}

containers::vector_of<method> kernel::get_available_methods(
    const std::string &url) const {
  containers::vector_of<method> methods;
  for (const auto &[route, controller] : *state_->get_router()->get_routes()) {
    if (auto [matches, bindings] = route_find(url, route); matches)
      methods.push_back(route.method_);
  }
  return methods;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

containers::async_of<
    std::tuple<shared<copper::models::request>,
               shared<copper::models::response>, response_generic>>
kernel::call(uuid session_id, boost::beast::string_view, const request &request,
             const std::string &ip, const uuid &request_id, long now) const {
  const std::string _url = url_from_request(request);
  uuid _user_id;

  auto _request =
      models::request_from_request(session_id, request_id, now, request);

  if (const auto _route = find_on_routes(request.method(), _url);
      _route.has_value()) {
    containers::unordered_map_of_strings bindings = _route.value().bindings_;

    _route.value().controller_->set_start_at(now);

    if (_route.value().controller_->configuration_.use_throttler_) {
      if (auto [can, TTL] = co_await state_->get_cache()->can_invoke(
          request, ip, _route.value().controller_->configuration_.rpm_);
          !can) {
        auto _service_response =
            response_too_many_requests(request, now, TTL, state_);

        auto _response = copper::models::response_from_response(
            session_id, _request, _service_response);

        co_return std::make_tuple(_request, _response, _service_response);
      }
    }

    if (_route.value().controller_->configuration_.use_auth_) {
      std::string bearer{request["Authorization"]};

      std::string token =
          boost::starts_with(bearer, "Bearer ") ? bearer.substr(7) : bearer;

      boost::optional<authentication_result> user_id;
      user_id = authentication_from_bearer(
          bearer, state_->get_configuration()->get()->app_key_);

      if (!user_id.has_value()) {
        auto _service_response = response_unauthorized(request, now, state_);

        auto _response = copper::models::response_from_response(
            session_id, _request, _service_response);

        _response->protected_ =
          _route.value().controller_->configuration_.use_protector_ == true;

        co_return std::make_tuple(_request, _response, _service_response);
      };

      _route.value().controller_->set_user(user_id.get().id_);
    }

    _route.value().controller_->set_bindings(bindings);
    _route.value().controller_->set_state(state_);

    if (_route.value().controller_->configuration_.use_validator_) {
      boost::system::error_code json_parse_error_code;

      json::value value =
          boost::json::parse(request.body(), json_parse_error_code);

      if (!json_parse_error_code) {
        _route.value().controller_->set_body(value);

        auto rules = _route.value().controller_->rules();

        if (auto validator = validator_make(rules, value);
            !validator->success_) {
          auto error_response =
              json::object({{"message", "The given data was invalid."},
                            {"errors", validator->errors_}});

          auto _service_response = _route.value().controller_->make_response(
              request, status_code::unprocessable_entity,
              serialize(error_response), "application/json");

          auto _response = copper::models::response_from_response(
              session_id, _request, _service_response);

          _response->protected_ =
            _route.value().controller_->configuration_.use_protector_ == true;

          co_return std::make_tuple(_request, _response, _service_response);
        }
      } else {
        auto error_response = json::object(
            {{"message", "The given data was invalid."},
             {"errors", {{"*", "The body must be a valid JSON."}}}});

        auto _service_response = _route.value().controller_->make_response(
            request, status_code::unprocessable_entity,
            serialize(error_response), "application/json");

        auto _response = copper::models::response_from_response(
            session_id, _request, _service_response);

        _response->protected_ =
          _route.value().controller_->configuration_.use_protector_ == true;

        co_return std::make_tuple(_request, _response, _service_response);
      }
    }

    try {
      auto _service_response =
          co_await _route.value().controller_->invoke(request);

      auto _response = copper::models::response_from_response(
          session_id, _request, _service_response);

      _response->protected_ =
        _route.value().controller_->configuration_.use_protector_ == true;

      co_return std::make_tuple(_request, _response, _service_response);
    } catch (std::exception &exception) {
      auto _service_response = response_exception(request, now, state_);

      auto _response = copper::models::response_from_response(
          session_id, _request, _service_response);

      _response->protected_ =
        _route.value().controller_->configuration_.use_protector_ == true;

      co_return std::make_tuple(_request, _response, _service_response);
    }
  }

  if (request.method() == method::options) {
    auto _available_verbs = get_available_methods(_url);

    auto _service_response =
        response_cors(request, now, _available_verbs, state_);

    auto _response = copper::models::response_from_response(
        session_id, _request, _service_response);

    co_return std::make_tuple(_request, _response, _service_response);
  }

  if (request_is_illegal(request)) {
    auto _service_response = response_bad_request(request, now, state_);

    auto _response = copper::models::response_from_response(
        session_id, _request, _service_response);

    co_return std::make_tuple(_request, _response, _service_response);
  }

  auto _service_response = response_not_found(request, now, state_);

  auto _response = copper::models::response_from_response(session_id, _request,
                                                          _service_response);

  co_return std::make_tuple(_request, _response, _service_response);
}
}  // namespace copper::components