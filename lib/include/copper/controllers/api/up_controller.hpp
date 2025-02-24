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
  /**
   * Invoke
   *
   * @param request
   * @return async_of<response>
   */
  components::containers::async_of<components::response> invoke(
      const components::request &request) override {
    auto now = components::chronos::now();
    const components::json::object data = {{"timestamp", now}};
    co_return make_response(request, components::status_code::ok,
                            serialize(data), "application/json");
  }
};

}  // namespace copper::controllers::api

#endif