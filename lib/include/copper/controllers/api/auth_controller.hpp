#ifndef COPPER_CONTROLLERS_API_AUTH_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_AUTH_CONTROLLER_HPP

#pragma once

#include <boost/lexical_cast.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers::api {

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
   * @param body
   * @param start_at
   * @return async_of<response>
   */
  components::containers::async_of<components::response> invoke(
      const components::request &request, const components::json::value &body,
      const components::containers::optional_of<
          components::authentication_result> & /*auth*/,
      const components::containers::unordered_map_of_strings & /*bindings*/,
      const long start_at) override {
    const std::string _email{body.as_object().at("email").as_string()};
    const std::string _password{body.as_object().at("password").as_string()};

    const auto _user =
        co_await state_->get_database()->get_user_by_email(_email);

    if (!_user.has_value()) {
      const components::json::object _errors = {
          {"message", "Email provided isn't registered."}};

      co_return make_response(request, components::status_code::unauthorized,
                              serialize(_errors), "application/json", start_at);
    }

    if (components::cipher_password_validator(_password,
                                              _user.value()->password_)) {
      std::string _token = components::authentication_to_bearer(
          boost::lexical_cast<components::uuid>(_user.value()->id_),
          state_->get_configuration()->get()->app_key_);

      const components::json::object _data = {{"token", _token}};

      auto _shared_token = std::make_shared<std::string>(_token.data());
      co_return make_response(request, components::status_code::ok,
                              serialize(_data), "application/json", start_at);
    }

    const components::json::object _errors = {
        {"message", "Password provided doesn't match."}};

    co_return make_response(request, components::status_code::unauthorized,
                            serialize(_errors), "application/json", start_at);
  }
};

}  // namespace copper::controllers::api

#endif