#pragma once

#include <boost/lexical_cast.hpp>
#include <copper/components/authentication.hpp>
#include <copper/components/cipher.hpp>
#include <copper/components/http_controller.hpp>
#include <copper/components/json.hpp>

namespace copper::controllers {
class auth_controller final : public copper::components::http_controller {
 public:
  copper::components::containers::map_of_strings rules() const override {
    return {
        {"*", "is_object"},
        {"email", "is_string"},
        {"password", "is_string"},
    };
  }

  copper::components::containers::async_of<copper::components::http_response>
  invoke(const copper::components::http_request &request) override {
    std::string email{data_.as_object().at("email").as_string()};
    std::string password{data_.as_object().at("password").as_string()};

    const auto user = co_await state_->get_database()->get_user_by_email(email);

    if (!user.has_value()) {
      const copper::components::json::object errors = {
          {"message", "Email provided isn't registered."}};

      co_return response(request,
                         copper::components::http_status_code::not_found,
                         serialize(errors), "application/json");
    }

    if (copper::components::cipher_password_validator(
            password, user.value()->password_)) {
      std::string token = copper::components::authentication_to_bearer(
          boost::lexical_cast<copper::components::uuid>(user.value()->id_),
          dotenv::getenv("APP_KEY"), "user");

      const copper::components::json::object data = {{"token", token}};

      auto shared_token = std::make_shared<std::string>(token.data());
      co_return response(request, copper::components::http_status_code::ok,
                         serialize(data), "application/json");
    }

    const copper::components::json::object errors = {
        {"message", "Password provided doesn't match."}};

    co_return response(request,
                       copper::components::http_status_code::unauthorized,
                       serialize(errors), "application/json");
  }
};

}  // namespace copper::controllers