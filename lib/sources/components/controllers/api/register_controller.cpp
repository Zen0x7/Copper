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

#include <copper/components/controllers/api/register_controller.hpp>
#include <copper/components/core.hpp>

namespace copper::components::controllers::api {
void register_controller::invoke(
    const shared<core>& core,
    const shared<controller_parameters>& parameters,
    const callback_of<res>& on_success,
    const callback_of<std::exception_ptr>& on_error) {
  LOG("[register_controller@invoke] scope_in");
  try {
    const auto& body = parameters->get_body().as_object();
    const std::string name{body.at("name").as_string()};
    const std::string email{body.at("email").as_string()};
    const std::string password{body.at("password").as_string()};

    core->database_->get_user_by_email(
        core, email,
        [this, core, name, email, password, parameters, on_success,
         on_error](const optional_of<shared<user>>& user_opt) {
          LOG("[register_controller@invoke] callback "
              "[database@get_user_by_email]");

          boost::ignore_unused(on_error);

          if (!user_opt.has_value()) {
            core->database_->create_user(
                core, name, email, password,
                [this, core, parameters, on_success](bool works) {
                  // LCOV_EXCL_START
                  if (!works) {
                    const json::object result = {
                        {"message", "User was not created."}};
                    on_success(make_response(
                        core, parameters, status_code::unauthorized,
                        serialize(result), "application/json"));
                    return;
                  }
                  // LCOV_EXCL_STOP

                  const json::object result = {
                      {"message", "User created successfully."}};
                  on_success(make_response(core, parameters, status_code::ok,
                                           serialize(result),
                                           "application/json"));
                });
          }
          // LCOV_EXCL_START
          else {
            const json::object errors = {
                {"message", "Email is already registered."}};
            on_success(make_response(core, parameters,
                                     status_code::unauthorized,
                                     serialize(errors), "application/json"));
          }
          // LCOV_EXCL_STOP
        });
  }
  // LCOV_EXCL_START
  catch (...) {
    LOG("[register_controller@invoke] catch");
    on_error(std::current_exception());
  }
  // LCOV_EXCL_STOP
  LOG("[register_controller@invoke] scope_out");
}
}  // namespace copper::components::controllers::api
