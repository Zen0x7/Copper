//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

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
#include <copper/components/response_bad_request.hpp>
#include <copper/components/response_cors.hpp>
#include <copper/components/response_exception.hpp>
#include <copper/components/response_not_found.hpp>
#include <copper/components/response_too_many_requests.hpp>
#include <copper/components/response_unauthorized.hpp>
#include <copper/components/route_find.hpp>
#include <copper/components/route_match.hpp>
#include <copper/components/router.hpp>
#include <copper/components/url.hpp>
#include <copper/components/validator.hpp>
#include <copper/models/request.hpp>
#include <copper/models/response.hpp>
#include <iostream>
#include <utility>

namespace copper::components {

containers::optional_of<kernel_result> kernel::find_on_routes(
    const method method, const std::string &url) const {
  auto _router = router::instance();

  for (const auto &[_route, _controller] : *_router->get_routes()) {
    if (auto [_matches, _bindings] = route_match(method, url, _route);
        _matches) {
      return kernel_result{
          .route_ = _route, .controller_ = _controller, .bindings_ = _bindings};
    }
  }

  return boost::none;
}

containers::vector_of<method> kernel::get_available_methods(
    const std::string &url) const {
  auto _router = router::instance();
  containers::vector_of<method> _methods;
  for (const auto &[_route, _controller] : *_router->get_routes()) {
    if (auto [_matches, _bindings] = route_find(url, _route); _matches)
      _methods.push_back(_route.method_);
  }
  return _methods;
  // LCOV_EXCL_START
}
// LCOV_EXCL_STOP

containers::async_of<std::tuple<shared<models::request>,
                                shared<models::response>, response_generic>>
kernel::call(uuid session_id, boost::beast::string_view, const request request,
             const std::string ip, const uuid request_id, long start_at) const {
  const std::string _url = url_from_request(request);

  auto _request =
      models::request_from_request(session_id, request_id, start_at, request);

  if (const auto _route = find_on_routes(request.method(), _url);
      _route.has_value()) {
    json::value _body = {};
    containers::unordered_map_of_strings _bindings = {};
    boost::optional<authentication_result> _user = boost::none;

    ++_route.value().controller_->count_;

    _bindings = _route.value().bindings_;

    if (_route.value().controller_->configuration_.use_throttler_) {
      if (auto [_can, _TTL] = co_await cache::instance()->can_invoke(
              request, ip, _route.value().controller_->configuration_.rpm_);
          !_can) {
        auto _service_response =
            response_too_many_requests(request, start_at, _TTL);

        auto _response =
            response_from_response(session_id, _request, _service_response);

        co_return std::make_tuple(_request, _response, _service_response);
      }
    }

    if (_route.value().controller_->configuration_.use_auth_) {
      std::string _bearer{request["Authorization"]};

      std::string _token =
          boost::starts_with(_bearer, "Bearer ") ? _bearer.substr(7) : _bearer;

      _user = authentication_from_bearer(
          _bearer, configuration::instance()->get()->app_key_);

      if (!_user.has_value()) {
        auto _service_response = response_unauthorized(request, start_at);

        auto _response =
            response_from_response(session_id, _request, _service_response);

        _response->protected_ =
            _route.value().controller_->configuration_.use_protector_ == true;

        co_return std::make_tuple(_request, _response, _service_response);
      }
    }

    if (_route.value().controller_->configuration_.use_validator_) {
      boost::system::error_code _json_error_code;

      _body = boost::json::parse(request.body(), _json_error_code);

      if (!_json_error_code) {
        if (auto _validator =
                validator_make(_route.value().controller_->rules(), _body);
            !_validator->success_) {
          auto _error_response =
              json::object({{"message", "The given data was invalid."},
                            {"errors", _validator->errors_}});

          auto _service_response = _route.value().controller_->make_response(
              boost::make_shared<controller_parameters>(request, _body, _user,
                                                        _bindings, start_at),
              status_code::unprocessable_entity, serialize(_error_response),
              "application/json");

          auto _response =
              response_from_response(session_id, _request, _service_response);

          _response->protected_ =
              _route.value().controller_->configuration_.use_protector_ == true;

          co_return std::make_tuple(_request, _response, _service_response);
        }
      } else {
        auto _error_response = json::object(
            {{"message", "The given data was invalid."},
             {"errors", {{"*", "The body must be a valid JSON."}}}});

        auto _service_response = _route.value().controller_->make_response(
            boost::make_shared<controller_parameters>(request, _body, _user,
                                                      _bindings, start_at),
            status_code::unprocessable_entity, serialize(_error_response),
            "application/json");

        auto _response =
            response_from_response(session_id, _request, _service_response);

        _response->protected_ =
            _route.value().controller_->configuration_.use_protector_ == true;

        co_return std::make_tuple(_request, _response, _service_response);
      }
    }

    try {
      auto _service_response = co_await _route.value().controller_->invoke(
          boost::make_shared<controller_parameters>(request, _body, _user,
                                                    _bindings, start_at));

      auto _response =
          response_from_response(session_id, _request, _service_response);

      _response->protected_ =
          _route.value().controller_->configuration_.use_protector_ == true;

      co_return std::make_tuple(_request, _response, _service_response);
    } catch (std::exception & /*exception*/) {
      auto _service_response = response_exception(request, start_at);

      auto _response =
          response_from_response(session_id, _request, _service_response);

      _response->protected_ =
          _route.value().controller_->configuration_.use_protector_ == true;

      co_return std::make_tuple(_request, _response, _service_response);
    }
  }

  if (request.method() == method::options) {
    auto _available_verbs = get_available_methods(_url);

    auto _service_response = response_cors(request, start_at, _available_verbs);

    auto _response =
        response_from_response(session_id, _request, _service_response);

    co_return std::make_tuple(_request, _response, _service_response);
  }

  if (request_is_illegal(request)) {
    auto _service_response = response_bad_request(request, start_at);

    auto _response =
        response_from_response(session_id, _request, _service_response);

    co_return std::make_tuple(_request, _response, _service_response);
  }

  auto _service_response = response_not_found(request, start_at);

  auto _response =
      response_from_response(session_id, _request, _service_response);

  co_return std::make_tuple(_request, _response, _service_response);
}

containers::async_of<shared<event>> kernel::handle(uuid session_id, uuid websocket_id, std::string message) {

    const auto _event = boost::make_shared<event>();

    boost::system::error_code _ec;

    const auto _value = boost::json::parse(message, _ec);

    if (_ec) {
        _event->status_code_ = status_code::unprocessable_entity;
        _event->data_ = {
            {"command", "ack"},
            {"message", "The given data be a valid JSON."},
            {"status", std::to_underlying(_event->status_code_) },
        };
        co_return _event;
    }

    containers::map_of_strings _rules = {
        {"*", "is_object"},
        {"id", "is_uuid"},
        {"command", "is_string"},
    };

    if (const auto _validator = validator_make(_rules, _value); !_validator->success_) {
        _event->status_code_ = status_code::unprocessable_entity;
        _event->data_ = {
            {"command", "ack"},
            {"message", "The given data was invalid."},
            {"errors", _validator->errors_},
            {"status", std::to_underlying(_event->status_code_)},
        };

        co_return _event;
    }

    co_return _event;
}
}  // namespace copper::components