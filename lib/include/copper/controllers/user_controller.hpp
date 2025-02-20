#ifndef COPPER_CONTROLLERS_USER_CONTROLLER_HPP
#define COPPER_CONTROLLERS_USER_CONTROLLER_HPP

#pragma once

#include <copper/components/chronos.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers {
class user_controller final : public components::http_controller {
 public:
  components::containers::async_of<components::http_response> invoke(
      const components::http_request &request) override {
    auto _user = co_await state_->get_database()->get_user_by_id(auth_id_);

    const components::json::object data = {
        {"id", _user->id_},
        {"name", _user->name_},
        {"email", _user->email_},
        {"email_verified_at", _user->email_verified_at_},
        {"created_at", _user->created_at_},
        {"updated_at", _user->updated_at_},
    };

    co_return response(request, components::http_status_code::ok,
                       serialize(data), "application/json");
  }
};

}  // namespace copper::controllers

#endif