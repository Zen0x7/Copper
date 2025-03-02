#ifndef COPPER_CONTROLLERS_API_AUTH_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_AUTH_CONTROLLER_HPP

#pragma once

//          Copyright Ian Torres 2025.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <boost/json/serialize.hpp>
#include <boost/lexical_cast.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/database.hpp>
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
   * @param parameters
   * @return async_of<response>
   */
  components::containers::async_of<components::response> invoke(
      const components::shared<components::controller_parameters> parameters)
      override {
    const std::string _email{
        parameters->get_body().as_object().at("email").as_string()};
    const std::string _password{
        parameters->get_body().as_object().at("password").as_string()};

    const auto _user =
        co_await components::database::instance()->get_user_by_email(_email);

    if (!_user.has_value()) {
      const components::json::object _errors = {
          {"message", "Email provided isn't registered."}};

      co_return make_response(parameters, components::status_code::unauthorized,
                              serialize(_errors), "application/json");
    }

    if (components::cipher_password_validator(_password,
                                              _user.value()->password_)) {
      std::string _token = components::authentication_to_bearer(
          boost::lexical_cast<components::uuid>(_user.value()->id_),
          components::configuration::instance()->get()->app_key_);

      const components::json::object _data = {{"token", _token}};

      auto _shared_token = std::make_shared<std::string>(_token.data());
      co_return make_response(parameters, components::status_code::ok,
                              serialize(_data), "application/json");
    }

    const components::json::object _errors = {
        {"message", "Password provided doesn't match."}};

    co_return make_response(parameters, components::status_code::unauthorized,
                            serialize(_errors), "application/json");
  }
};

}  // namespace copper::controllers::api

#endif