#ifndef COPPER_CONTROLLERS_AUTH_CONTROLLER_HPP
#define COPPER_CONTROLLERS_AUTH_CONTROLLER_HPP

#pragma once

#include <boost/lexical_cast.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers {

/**
 * Auth controller
 */
class auth_controller final : public components::controller {
 public:
  /**
   * Rules
   *
   * @return map_of_strings
   */
  components::containers::map_of_strings rules() const override {
    return {
        {"*", "is_object"},
        {"email", "is_string"},
        {"password", "is_string"},
    };
  }

  /**
   * Invoke
   *
   * @param request
   * @return async_of<http_response>
   */
  components::containers::async_of<components::http_response> invoke(
      const components::http_request &request) override {
    std::string email{body_.as_object().at("email").as_string()};
    std::string password{body_.as_object().at("password").as_string()};

    const auto user = co_await state_->get_database()->get_user_by_email(email);

    if (!user.has_value()) {
      const components::json::object errors = {
          {"message", "Email provided isn't registered."}};

      co_return make_response(request, components::status_code::unauthorized,
                              serialize(errors), "application/json");
    }

    if (components::cipher_password_validator(password,
                                              user.value()->password_)) {
      std::string token = components::authentication_to_bearer(
          boost::lexical_cast<components::uuid>(user.value()->id_),
          state_->get_configuration()->get()->app_key_);

      const components::json::object data = {{"token", token}};

      auto shared_token = std::make_shared<std::string>(token.data());
      co_return make_response(request, components::status_code::ok,
                              serialize(data), "application/json");
    }

    const components::json::object errors = {
        {"message", "Password provided doesn't match."}};

    co_return make_response(request, components::status_code::unauthorized,
                            serialize(errors), "application/json");
  }
};

}  // namespace copper::controllers

#endif