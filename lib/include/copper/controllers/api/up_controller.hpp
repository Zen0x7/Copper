#ifndef COPPER_CONTROLLERS_API_UP_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_UP_CONTROLLER_HPP

#pragma once

#include <copper/components/cipher.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers::api {

/**
 * UP Controller
 */
class up_controller final : public components::controller {
 public:
  using controller::controller;

  /**
   * Invoke
   *
   * @param parameters
   * @return async_of<response>
   */
  components::containers::async_of<components::response> invoke(
      const components::shared<components::controller_parameters> &parameters)
      override {
    auto _now = components::chronos::now();
    const components::json::object _data = {{"timestamp", _now}};
    co_return make_response(parameters, components::status_code::ok,
                            serialize(_data), "application/json");
  }
};

}  // namespace copper::controllers::api

#endif