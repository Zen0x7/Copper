#ifndef COPPER_CONTROLLERS_API_USER_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_USER_CONTROLLER_HPP

#pragma once

#include <copper/components/chronos.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers::api {
using namespace copper::components;

/**
 * User controller
 */
class user_controller final : public controller {
 public:
  /**
   * Invoke
   *
   * @param parameters
   * @return async_of<response>
   */
  containers::async_of<response> invoke(
      const shared<controller_parameters> &parameters) override {
    const auto _user = co_await database::instance()->get_user_by_id(
        parameters->get_auth().value().id_);

    const json::object _data = {
        {"id", _user.value()->id_},
        {"name", _user.value()->name_},
        {"email", _user.value()->email_},
        {"email_verified_at", _user.value()->email_verified_at_},
        {"created_at", _user.value()->created_at_},
        {"updated_at", _user.value()->updated_at_},
    };

    co_return make_response(parameters, status_code::ok, serialize(_data),
                            "application/json");
  }
};

}  // namespace copper::controllers::api

#endif