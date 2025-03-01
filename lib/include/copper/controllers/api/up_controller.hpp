#ifndef COPPER_CONTROLLERS_API_UP_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_UP_CONTROLLER_HPP

#pragma once

#include <copper/components/cipher.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers::api {
using namespace copper::components;

/**
 * UP Controller
 */
class up_controller final : public controller {
 public:
  /**
   * Invoke
   *
   * @param parameters
   * @return async_of<response>
   */
  containers::async_of<response> invoke(
      const shared<controller_parameters> &parameters) override {
    auto _now = chronos::now();
    const json::object _data = {{"timestamp", _now}};
    co_return make_response(parameters, status_code::ok, serialize(_data),
                            "application/json");
  }
};

}  // namespace copper::controllers::api

#endif