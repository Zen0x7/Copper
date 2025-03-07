//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/algorithm/string/predicate.hpp>
#include <boost/smart_ptr.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/response_shared_handler.hpp>
#include <copper/components/views.hpp>

namespace copper::components {
// LCOV_EXCL_START
void controller::set_configuration(
    const controller_configuration configuration) {
  configuration_ = configuration;
}

response controller::make_response(
    const shared<controller_parameters> &parameters, const status_code status,
    const std::string &data, const char *type) const {
  response _response{status, parameters->get_request().version()};

  const auto _allowed_origins =
      configuration::instance()->get()->http_allowed_origins_;

  _response.set(fields::access_control_allow_origin, _allowed_origins);

  _response.keep_alive(parameters->get_request().keep_alive());
  _response.set(fields::content_type, type);

  if (!parameters->get_request()["Accept-Encoding"].empty() &&
      boost::contains(parameters->get_request()["Accept-Encoding"], "gzip")) {
    _response.body() = gunzip_compress(data);
    _response.set(fields::content_encoding, "gzip");
  } else {
    _response.body() = data;
  }

  _response.prepare_payload();

  const auto _resolved_at = chronos::now();
  _response.set("X-Server", "Copper");
  _response.set("X-Time",
                std::to_string(_resolved_at - parameters->get_start_at()));

  return _response;
}

response controller::make_view(const shared<controller_parameters> &parameters,
                               const status_code status,
                               const std::string &view, const json::json &data,
                               const char *type) const {
  response _response{status, parameters->get_request().version()};

  const auto _allowed_origins =
      configuration::instance()->get()->http_allowed_origins_;

  _response.set(fields::access_control_allow_origin, _allowed_origins);

  _response.keep_alive(parameters->get_request().keep_alive());
  _response.set(fields::content_type, type);

  if (!parameters->get_request()["Accept-Encoding"].empty() &&
      boost::contains(parameters->get_request()["Accept-Encoding"], "gzip")) {
    _response.body() = gunzip_compress(views::instance()->render(view, data));
    _response.set(fields::content_encoding, "gzip");
  } else {
    _response.body() = views::instance()->render(view, data);
  }

  _response.prepare_payload();

  const auto _resolved_at = chronos::now();
  _response.set("X-Server", "Copper");
  _response.set("X-Time",
                std::to_string(_resolved_at - parameters->get_start_at()));

  return _response;
}
// LCOV_EXCL_STOP
}  // namespace copper::components