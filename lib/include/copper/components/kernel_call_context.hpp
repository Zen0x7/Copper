#ifndef COPPER_COMPONENTS_KERNEL_CALL_CONTEXT_HPP
#define COPPER_COMPONENTS_KERNEL_CALL_CONTEXT_HPP

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
#include <copper/components/core.hpp>
#include <copper/components/event.hpp>
#include <copper/components/kernel.hpp>
#include <copper/components/req.hpp>
#include <copper/components/request.hpp>
#include <copper/components/response_exception.hpp>
#include <copper/components/response_generic.hpp>
#include <copper/components/route.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {

/**
 * Kernel call context
 */
struct kernel_call_context : shared_enabled<kernel_call_context> {
  /**
   * Core
   */
  shared<core> core_;

  /**
   * Server ID
   */
  uuid server_id_;

  /**
   * Connection
   */
  shared<connection> connection_;

  /**
   * Request
   */
  req request_;

  /**
   * Start at
   */
  long start_at_;

  /**
   * Callback
   */
  callback_of<res> callback_;

  /**
   * URL
   */
  std::string url_;

  /**
   * Route
   */
  boost::optional<kernel_result> route_;

  /**
   * Bindings
   */
  unordered_map_of_strings bindings_;

  /**
   * Body
   */
  json::value body_;

  /**
   * User
   */
  boost::optional<authentication_result> user_;

  /**
   * Constructor
   *
   * @param core
   * @param server_id
   * @param connection
   * @param request
   * @param start_at
   * @param callback
   */
  explicit kernel_call_context(const shared<core>& core,
                               const uuid server_id,
                               const shared<connection>& connection,
                               const req& request,
                               long start_at,
                               callback_of<res> callback);

  /**
   * Start
   *
   * @return void
   */
  void start();

  /**
   * Check throttle
   *
   * @return void
   */
  void check_throttle();

  /**
   * Check auth
   *
   * @return void
   */
  void check_auth();

  /**
   * Check validation
   *
   * @return void
   */
  void check_validation();

  /**
   * Invoke controller
   *
   * @return void
   */
  void invoke_controller();

  /**
   * Build parameters
   *
   * @return shared<controller_parameters>
   */
  shared<controller_parameters> build_parameters();

  void finalize(const res& res, bool is_protected = false) const;

  /**
   * Return not found
   *
   * @return void
   */
  void return_not_found() const;
};

}  // namespace copper::components

#endif
