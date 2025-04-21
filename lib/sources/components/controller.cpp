// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <boost/algorithm/string/predicate.hpp>
#include <boost/smart_ptr.hpp>
#include <copper/components/chronos.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/core.hpp>
#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/response_shared_handler.hpp>
#include <copper/components/views.hpp>

namespace copper::components {

// LCOV_EXCL_START
res controller::make_response(const shared<core>& core,
                              const shared<controller_parameters>& parameters,
                              const status_code status,
                              const std::string& data,
                              const char* type) const {
  res _response{status, parameters->get_request().version()};

  const auto _allowed_origins =
      core->configuration_->get()->http_allowed_origins_;

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

res controller::make_view(const shared<core>& core,
                          const shared<controller_parameters>& parameters,
                          const status_code status,
                          const std::string& view,
                          const json::json& data,
                          const char* type) const {
  res _response{status, parameters->get_request().version()};

  _response.set(fields::access_control_allow_origin, "*");

  _response.keep_alive(parameters->get_request().keep_alive());
  _response.set(fields::content_type, type);

  if (!parameters->get_request()["Accept-Encoding"].empty() &&
      boost::contains(parameters->get_request()["Accept-Encoding"], "gzip")) {
    _response.body() = gunzip_compress(core->views_->render(view, data));
    _response.set(fields::content_encoding, "gzip");
  } else {
    _response.body() = core->views_->render(view, data);
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
