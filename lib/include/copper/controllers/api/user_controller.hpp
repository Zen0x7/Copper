#ifndef COPPER_CONTROLLERS_API_USER_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_USER_CONTROLLER_HPP

#pragma once

#include <copper/components/chronos.hpp>
#include <copper/components/cipher.hpp>
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
   * @param request
   * @param auth
   * @param start_at
   * @return async_of<response>
   */
  components::containers::async_of<components::response> invoke(
      const components::request &request,
      const components::json::value & /*body*/,
      const components::containers::optional_of<
          components::authentication_result> &auth,
      const components::containers::unordered_map_of_strings & /*bindings*/,
      const long start_at) override {
    const auto _user =
        co_await state_->get_database()->get_user_by_id(auth.value().id_);

    const components::json::object _data = {
        {"id", _user->id_},
        {"name", _user->name_},
        {"email", _user->email_},
        {"email_verified_at", _user->email_verified_at_},
        {"created_at", _user->created_at_},
        {"updated_at", _user->updated_at_},
    };

    co_return make_response(request, components::status_code::ok,
                            serialize(_data), "application/json", start_at);
  }
};

}  // namespace copper::controllers::api

#endif