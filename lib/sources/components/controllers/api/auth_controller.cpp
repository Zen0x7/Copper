// Copyright (C) 2025 Ian Torres
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#include <copper/components/controllers/api/auth_controller.hpp>
#include <copper/components/core.hpp>

namespace copper::components::controllers::api {
void auth_controller::invoke(const shared<core>& core,
                             const shared<controller_parameters> parameters,
                             const callback_of<res> on_success,
                             const callback_of<std::exception_ptr> on_error) {
  try {
    const auto& body = parameters->get_body().as_object();
    const std::string email{body.at("email").as_string()};
    const std::string password{body.at("password").as_string()};

    core->database_->get_user_by_email(
        core, email,
        [this, core, email, password, parameters, on_success,
         on_error](optional_of<shared<user>> user_opt) {
          boost::ignore_unused(email, on_error);

          // LCOV_EXCL_START
          if (!user_opt.has_value()) {
            const json::object errors = {
                {"message", "Email provided isn't registered."}};
            return on_success(
                make_response(core, parameters, status_code::unauthorized,
                              serialize(errors), "application/json"));
          }
          // LCOV_EXCL_STOP

          const auto& user = user_opt.value();
          if (cipher_password_validator(password, user->password_)) {
            std::string token =
                authentication_to_bearer(boost::lexical_cast<uuid>(user->id_),
                                         core->configuration_->get()->app_key_);

            const json::object data = {{"token", token}};
            return on_success(make_response(core, parameters, status_code::ok,
                                            serialize(data),
                                            "application/json"));
          }

          // LCOV_EXCL_START
          const json::object errors = {
              {"message", "Password provided doesn't match."}};
          return on_success(
              make_response(core, parameters, status_code::unauthorized,
                            serialize(errors), "application/json"));
        });
  } catch (...) {
    on_error(std::current_exception());
  }
  // LCOV_EXCL_STOP
}
}  // namespace copper::components::controllers::api
