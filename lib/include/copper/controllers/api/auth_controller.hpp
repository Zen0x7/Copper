#ifndef COPPER_CONTROLLERS_API_AUTH_CONTROLLER_HPP
#define COPPER_CONTROLLERS_API_AUTH_CONTROLLER_HPP

#pragma once

#include <boost/lexical_cast.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/configuration.hpp>
#include <copper/components/controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers::api {

using namespace copper::components;

/**
 * Auth controller
 */
class auth_controller final : public controller {
 public:
  using controller::controller;

  /**
   * Rules
   *
   * @return map_of_strings
   */
  containers::map_of_strings rules() const override {
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
  containers::async_of<response> invoke(
      const shared<controller_parameters>& parameters) override {
    const std::string _email{
        parameters->get_body().as_object().at("email").as_string()};
    const std::string _password{
        parameters->get_body().as_object().at("password").as_string()};

    const auto _user =
        co_await state_->get_database()->get_user_by_email(_email);

    if (!_user.has_value()) {
      const json::object _errors = {
          {"message", "Email provided isn't registered."}};

      co_return make_response(parameters, status_code::unauthorized,
                              serialize(_errors), "application/json");
    }

    if (cipher_password_validator(_password, _user.value()->password_)) {
      std::string _token = authentication_to_bearer(
          boost::lexical_cast<uuid>(_user.value()->id_),
          configuration::instance()->get()->app_key_);

      const json::object _data = {{"token", _token}};

      auto _shared_token = std::make_shared<std::string>(_token.data());
      co_return make_response(parameters, status_code::ok, serialize(_data),
                              "application/json");
    }

    const json::object _errors = {
        {"message", "Password provided doesn't match."}};

    co_return make_response(parameters, status_code::unauthorized,
                            serialize(_errors), "application/json");
  }
};

}  // namespace copper::controllers::api

#endif