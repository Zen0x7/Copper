#ifndef COPPER_COMPONENTS_CONTROLLERS_API_REGISTER_CONTROLLER_HPP
#define COPPER_COMPONENTS_CONTROLLERS_API_REGISTER_CONTROLLER_HPP

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

#include <boost/json/serialize.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/database.hpp>
#include <copper/components/json.hpp>

namespace copper::components::controllers::api {

/**
 * Register controller
 */
class register_controller final : public controller {
 public:
  /**
   * Rules
   *
   * @return map_of_strings
   */
  map_of_strings rules() const override {
    return {
        {"*", "is_object"},
        {"name", "is_string"},
        {"email", "is_string"},
        {"password", "is_string,required"},
    };
  }

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
              const shared<controller_parameters>& parameters,
              const callback_of<res>& on_success,
              const callback_of<std::exception_ptr>& on_error) override;
};

}  // namespace copper::components::controllers::api

#endif
