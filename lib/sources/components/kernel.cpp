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
#include <copper/components/url.hpp>
#include <copper/components/validator.hpp>
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
  for (const auto &route :
       *state_->get_router()->get_routes() | std::views::keys) {
    if (auto [matches, bindings] = route_find(url, route); matches)
      methods.push_back(route.method_);
  }
  return methods;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

containers::async_of<std::tuple<shared<models::request>,
                                shared<models::response>, response_generic>>
kernel::call(uuid session_id, boost::beast::string_view, const request &request,
             const std::string &ip, const uuid &request_id,
             long start_at) const {
  const std::string _url = url_from_request(request);

  auto _request =
      models::request_from_request(session_id, request_id, start_at, request);

  if (const auto _route = find_on_routes(request.method(), _url);
      _route.has_value()) {
    json::value _body;
    containers::unordered_map_of_strings _bindings;
    boost::optional<authentication_result> _user;

    if (_route.value().controller_->count_ == 0) {
      _route.value().controller_->set_state(state_);
    }

    ++_route.value().controller_->count_;

    _bindings = _route.value().bindings_;

    if (_route.value().controller_->configuration_.use_throttler_) {
      if (auto [can, TTL] = co_await state_->get_cache()->can_invoke(
              request, ip, _route.value().controller_->configuration_.rpm_);
          !can) {
        auto _service_response =
            response_too_many_requests(request, start_at, TTL, state_);

        auto _response =
            response_from_response(session_id, _request, _service_response);

        co_return std::make_tuple(_request, _response, _service_response);
      }
    }

    if (_route.value().controller_->configuration_.use_auth_) {
      std::string bearer{request["Authorization"]};

      std::string token =
          boost::starts_with(bearer, "Bearer ") ? bearer.substr(7) : bearer;

      _user = authentication_from_bearer(
          bearer, state_->get_configuration()->get()->app_key_);

      if (!_user.has_value()) {
        auto _service_response =
            response_unauthorized(request, start_at, state_);

        auto _response =
            response_from_response(session_id, _request, _service_response);

        _response->protected_ =
            _route.value().controller_->configuration_.use_protector_ == true;

        co_return std::make_tuple(_request, _response, _service_response);
      }
    }

    if (_route.value().controller_->configuration_.use_validator_) {
      boost::system::error_code _json_parse_error_code;

      _body = boost::json::parse(request.body(), _json_parse_error_code);

      if (!_json_parse_error_code) {
        if (auto _validator =
                validator_make(_route.value().controller_->rules(), _body);
            !_validator->success_) {
          auto _error_response =
              json::object({{"message", "The given data was invalid."},
                            {"errors", _validator->errors_}});

          auto _service_response = _route.value().controller_->make_response(
              request, status_code::unprocessable_entity,
              serialize(_error_response), "application/json");

          auto _response =
              response_from_response(session_id, _request, _service_response);

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

        auto _response =
            response_from_response(session_id, _request, _service_response);

        _response->protected_ =
            _route.value().controller_->configuration_.use_protector_ == true;

        co_return std::make_tuple(_request, _response, _service_response);
      }
    }

    try {
      auto _service_response = co_await _route.value().controller_->invoke(
          request, _body, _user, _bindings, start_at);

      auto _response =
          response_from_response(session_id, _request, _service_response);

      _response->protected_ =
          _route.value().controller_->configuration_.use_protector_ == true;

      co_return std::make_tuple(_request, _response, _service_response);
    } catch (std::exception &exception) {
      auto _service_response = response_exception(request, start_at, state_);

      auto _response =
          response_from_response(session_id, _request, _service_response);

      _response->protected_ =
          _route.value().controller_->configuration_.use_protector_ == true;

      co_return std::make_tuple(_request, _response, _service_response);
    }
  }

  if (request.method() == method::options) {
    auto _available_verbs = get_available_methods(_url);

    auto _service_response =
        response_cors(request, start_at, _available_verbs, state_);

    auto _response =
        response_from_response(session_id, _request, _service_response);

    co_return std::make_tuple(_request, _response, _service_response);
  }

  if (request_is_illegal(request)) {
    auto _service_response = response_bad_request(request, start_at, state_);

    auto _response =
        response_from_response(session_id, _request, _service_response);

    co_return std::make_tuple(_request, _response, _service_response);
  }

  auto _service_response = response_not_found(request, start_at, state_);

  auto _response =
      response_from_response(session_id, _request, _service_response);

  co_return std::make_tuple(_request, _response, _service_response);
}
}  // namespace copper::components