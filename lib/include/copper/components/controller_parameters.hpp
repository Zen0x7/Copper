#ifndef COPPER_COMPONENTS_CONTROLLER_PARAMETERS_HPP
#define COPPER_COMPONENTS_CONTROLLER_PARAMETERS_HPP

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

#include <copper/components/authentication.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/json.hpp>
#include <copper/components/req.hpp>
#include <copper/components/shared.hpp>

namespace copper::components {
class controller_parameters : public shared_enabled<controller_parameters> {
  /**
   * Request
   */
  req request_;

  /**
   * Body
   */
  json::value body_;

  /**
   * Auth
   */
  containers::optional_of<authentication_result> auth_;

  /**
   * Bindings
   */
  containers::unordered_map_of_strings bindings_;

  /**
   * Start at
   */
  long start_at_;

 public:
  /**
   * Constructor
   */
  controller_parameters(
      req request,
      json::value body,
      const containers::optional_of<authentication_result>& auth,
      containers::unordered_map_of_strings bindings,
      const long& start_at)
      : request_(std::move(request)),
        body_(std::move(body)),
        auth_(auth),
        bindings_(std::move(bindings)),
        start_at_(start_at) {}

  /**
   * Get request
   *
   * @return request
   */
  const req& get_request() const { return request_; }

  /**
   * Get body
   *
   * @return json::value
   */
  const json::value& get_body() const { return body_; }

  /**
   * Get auth
   *
   * @return optional_of<authentication_result>
   */
  const containers::optional_of<authentication_result>& get_auth() const {
    return auth_;
  }

  /**
   * Get bindings
   *
   * @return unordered_map_of_strings
   */
  const containers::unordered_map_of_strings& get_bindings() const {
    return bindings_;
  }

  /**
   * Get start at
   *
   * @return long
   */
  const long& get_start_at() const { return start_at_; };
};

}  // namespace copper::components

#endif
