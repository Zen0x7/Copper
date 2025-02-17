#pragma once

#include <copper/components/http_controller.hpp>
#include <copper/components/json.hpp>
#include <copper/components/cipher.hpp>

namespace app::controllers {
    class auth_controller final : public copper::components::http_controller {
    public:
      bool requires_limitation() const override { return true; }

      bool requires_data() const override { return true; }

      int requests_per_minute() const override { return 5; }

      copper::components::containers::map_of_strings validate_data() const override {
        return {
          {"*",        "is_object"},
          {"email",    "is_string"},
          {"password", "is_string"},
        };
      }

      copper::components::http_response invoke(const copper::components::http_request &request) override {

        std::string email{data_.as_object().at("email").as_string()};
        std::string password{data_.as_object().at("password").as_string()};


        const auto user = state_->get_database()->get_user_by_email(email);

        if (!user.has_value()) {
          const copper::components::json::object errors = {{"message", "Email provided isn't registered."}};


          return response(request, copper::components::http_status_code::not_found, serialize(errors),
                          "application/json");
        }

        if (copper::components::cipher_password_validator(password, user.value().password_)) {
          // LCOV_EXC_START
          const copper::components::json::object header = {
            {"srv", "Copper"},
            {"aut", "Ian Torres"},
            {"alg", "HS256"},
            {"typ", "JWT"},
          };
          // LCOV_EXCL_STOP

          const std::string id_ = boost::uuids::to_string(user.value().id_);
          auto now = std::chrono::system_clock::now();
          auto expires_at = now + std::chrono::days(7);
          std::string type = "user";
          // LCOV_EXC_START
          const copper::components::json::object payload = {
            {"sub", id_},
            {"typ", type},
            {"iat", copper::components::chronos::to_timestamp(now)},
            {"exp", copper::components::chronos::to_timestamp(expires_at)},
          };
          // LCOV_EXCL_STOP

          const std::string header_ = copper::components::base64url_encode(serialize(header));
          const std::string payload_ = copper::components::base64url_encode(serialize(payload));
          const std::string signature_ = copper::components::base64url_encode(
            copper::components::cipher_hmac(header_ + "." + payload_, dotenv::getenv("APP_KEY")));
          std::string token = "Bearer " + header_ + "." + payload_ + "." + signature_;

          const copper::components::json::object data = {{"token",token}};

          auto shared_token = std::make_shared<std::string>(token.data());
          return response(request, copper::components::http_status_code::ok, serialize(data), "application/json");
        }

        const copper::components::json::object errors = {{"message", "Password provided doesn't match."}};

        return response(request, copper::components::http_status_code::unauthorized, serialize(errors),
                        "application/json");
      }
    };
}