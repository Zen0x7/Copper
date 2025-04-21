#ifndef COPPER_COMPONENTS_CONTROLLER_HPP
#define COPPER_COMPONENTS_CONTROLLER_HPP

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

#include <boost/atomic.hpp>
#include <boost/json/serialize.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/containers.hpp>
#include <copper/components/controller_configuration.hpp>
#include <copper/components/controller_parameters.hpp>
#include <copper/components/json.hpp>
#include <copper/components/res.hpp>
#include <copper/components/shared.hpp>
#include <copper/components/status_code.hpp>
#include <copper/components/uuid.hpp>

namespace copper::components {
using namespace containers;

class core;

/**
 * Controller
 */
class controller : public shared_enabled<controller> {
 public:
  /**
   * Destructor
   */
  virtual ~controller() = default;

  /**
   * Auth ID
   */
  uuid auth_id_;

  /**
   * Start at
   */
  long start_at_ = 0;

  /**
   * Count
   */
  boost::atomic<unsigned long long> count_;

  /**
   * Controller configuration
   */
  controller_configuration controller_configuration_;

  /**
   * Invoke
   *
   * @return async_of<response>
   */
  // LCOV_EXCL_START
  virtual void invoke(const shared<core>& /*core*/,
                      const shared<controller_parameters> /*parameters*/,
                      const callback_of<res> on_success,
                      const callback_of<std::exception_ptr> /*on_error*/) {
    const res _res;
    on_success(_res);
  }
  // LCOV_EXCL_STOP

  /**
   * Rules
   *
   * @return map_of_strings
   */
  // LCOV_EXCL_START
  virtual map_of_strings rules() const { return {}; }
  // LCOV_EXCL_STOP

  /**
   * Set controller configuration
   *
   * @param configuration
   */
  void set_controller_configuration(controller_configuration configuration) {
    controller_configuration_ = configuration;
  }

  /**
   * Make response
   *
   * @param core
   * @param parameters
   * @param status
   * @param data
   * @param type
   * @return response
   */
  res make_response(const shared<core>& core,
                    const shared<controller_parameters>& parameters,
                    status_code status,
                    const std::string& data,
                    const char* type = "text/html") const;

  /**
   * Make view
   *
   * @param core
   * @param parameters
   * @param status
   * @param view
   * @param data
   * @param type
   * @return response
   */
  res make_view(const shared<core>& core,
                const shared<controller_parameters>& parameters,
                status_code status,
                const std::string& view,
                const json::json& data,
                const char* type = "text/html") const;
};

}  // namespace copper::components

#endif
