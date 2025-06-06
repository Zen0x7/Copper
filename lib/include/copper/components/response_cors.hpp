#ifndef COPPER_COMPONENTS_RESPONSE_CORS_HPP
#define COPPER_COMPONENTS_RESPONSE_CORS_HPP

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

#include <copper/components/containers.hpp>
#include <copper/components/method.hpp>
#include <copper/components/req.hpp>
#include <copper/components/res.hpp>
#include <copper/components/response_shared_handler.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
class core;

/**
 * Generates response on CORS
 *
 * @param core
 * @param request
 * @param start_at
 * @param methods
 * @return res
 */
inline res res_cors(const shared<core>& core,
                    const req& request,
                    long start_at,
                    const vector_of<method>& methods) {
  res _response{
      methods.empty() ? status_code::method_not_allowed : status_code::ok,
      request.version()};

  vector_of<std::string> _authorized_methods;
  for (const auto& _verb : methods)
    _authorized_methods.push_back(to_string(_verb));
  const auto _methods_as_string = boost::join(_authorized_methods, ",");
  _response.set(fields::access_control_allow_methods,
                methods.empty() ? "" : _methods_as_string);

  const std::string _allowed_headers = "Accept,Authorization,Content-Type";
  _response.set(fields::access_control_allow_headers, _allowed_headers);

  return res_shared_handler(core, request, _response, start_at);
}

}  // namespace copper::components

#endif
