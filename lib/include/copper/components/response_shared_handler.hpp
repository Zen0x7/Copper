#ifndef COPPER_COMPONENTS_SHARED_HANDLER_HPP
#define COPPER_COMPONENTS_SHARED_HANDLER_HPP

#pragma once

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

#include <copper/components/fields.hpp>
#include <copper/components/gunzip.hpp>
#include <copper/components/req.hpp>
#include <copper/components/res.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
class core;

/**
 * Response shared handler
 *
 * @param core
 * @param request
 * @param response
 * @param start_at
 * @param ttl
 * @return res
 */
inline res res_shared_handler(const shared<core>& core,
                              const req& request,
                              res& response,
                              long start_at,
                              int ttl = -1) {
  const bool _requires_html =
      request.count(fields::accept) > 0 &&
      boost::contains(request.at(fields::accept), "html");

  const auto _allowed_origins =
      core->configuration_->get()->http_allowed_origins_;

  response.set(fields::access_control_allow_origin, _allowed_origins);

  response.version(request.version());
  response.keep_alive(request.keep_alive());

  if (_requires_html) {
    response.set(fields::content_type, "text/html");
  } else {
    response.set(fields::content_type, "application/json");
  }

  if (!request["Accept-Encoding"].empty() &&
      boost::contains(request["Accept-Encoding"], "gzip")) {
    response.set(fields::content_encoding, "gzip");
    if (_requires_html) {
      response.body() = gunzip_compress(
          core->views_->render(std::to_string(response.result_int())));
    } else {
      response.body() = gunzip_compress("{}");
    }
  } else {
    if (_requires_html) {
      response.body() =
          core->views_->render(std::to_string(response.result_int()));
    } else {
      response.body() = "{}";
    }
  }

  response.prepare_payload();

  const auto _now = chronos::now();
  response.set("X-Server", "Copper");
  response.set("X-Time", std::to_string(_now - start_at));

  if (ttl != -1) {
    response.set("X-Rate-Until", std::to_string(ttl));
  }

  return response;
}
}  // namespace copper::components

#endif
