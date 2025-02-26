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
   * @param start_at
   * @return async_of<response>
   */
  components::containers::async_of<components::response> invoke(
      const components::request &request,
      const components::json::value & /*body*/,
      const components::containers::optional_of<
          components::authentication_result> & /*auth*/,
      const components::containers::unordered_map_of_strings & /*bindings*/,
      const long start_at) override {
    auto _now = components::chronos::now();
    const components::json::object _data = {{"timestamp", _now}};
    co_return make_response(request, components::status_code::ok,
                            serialize(_data), "application/json", start_at);
  }
};

}  // namespace copper::controllers::api

#endif