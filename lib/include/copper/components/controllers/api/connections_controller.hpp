#ifndef COPPER_CONTROLLERS_API_WEBSOCKETS_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_WEBSOCKETS_CONTROLLER_HPP

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

#include <copper/components/connection.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/json.hpp>
#include <copper/components/state.hpp>
#include <iostream>

namespace copper::components::controllers::api {

/**
 * Connections controller
 */
class connections_controller final : public controller {
 public:
  /**
   * Invoke
   *
   * @param core
   * @param parameters
   * @param on_success
   * @param on_error
   * @return void
   */
  void invoke(const shared<core>& core,
              shared<controller_parameters> parameters,
              callback_of<res> on_success,
              callback_of<std::exception_ptr> on_error) override;
};

}  // namespace copper::components::controllers::api

#endif
