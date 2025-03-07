#ifndef COPPER_CONTROLLERS_API_UP_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_UP_CONTROLLER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/chronos.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers::api {

/**
 * UP Controller
 */
class up_controller final : public components::controller {
 public:
  /**
   * Invoke
   *
   * @param parameters
   * @return async_of<response>
   */
  components::containers::async_of<components::response> invoke(
      const components::shared<components::controller_parameters> parameters)
      override {
    auto _now = components::chronos::now();
    const components::json::object _data = {{"timestamp", _now}};
    co_return make_response(parameters, components::status_code::ok,
                            serialize(_data), "application/json");
  }
};

}  // namespace copper::controllers::api

#endif