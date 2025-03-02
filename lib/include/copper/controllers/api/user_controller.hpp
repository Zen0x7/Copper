#ifndef COPPER_CONTROLLERS_API_USER_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_USER_CONTROLLER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <copper/components/controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers::api {

/**
 * User controller
 */
class user_controller final : public components::controller {
 public:
  /**
   * Invoke
   *
   * @param parameters
   * @return async_of<response>
   */
  components::containers::async_of<components::response> invoke(
      const components::shared<components::controller_parameters> &parameters)
      override {
    const auto _user =
        co_await components::database::instance()->get_user_by_id(
            parameters->get_auth().value().id_);

    const components::json::object _data = {
        {"id", _user.value()->id_},
        {"name", _user.value()->name_},
        {"email", _user.value()->email_},
        {"email_verified_at", _user.value()->email_verified_at_},
        {"created_at", _user.value()->created_at_},
        {"updated_at", _user.value()->updated_at_},
    };

    co_return make_response(parameters, components::status_code::ok,
                            serialize(_data), "application/json");
  }
};

}  // namespace copper::controllers::api

#endif