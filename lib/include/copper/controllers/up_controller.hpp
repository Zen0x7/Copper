#ifndef COPPER_CONTROLLERS_UP_CONTROLLER_HPP
#define COPPER_CONTROLLERS_UP_CONTROLLER_HPP

#pragma once

#include <copper/components/cipher.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers {

/**
 * UP Controller
 */
class up_controller final : public components::http_controller {
 public:
  /**
   * Invoke
   *
   * @param request
   * @return async_of<http_response>
   */
  components::containers::async_of<components::http_response> invoke(
      const components::http_request &request) override {
    auto now = components::chronos::now();
    const components::json::object data = {{"timestamp", now}};
    co_return make_response(request, components::http_status_code::ok,
                            serialize(data), "application/json");
  }
};

}  // namespace copper::controllers

#endif