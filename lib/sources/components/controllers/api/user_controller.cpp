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

#include <copper/components/controllers/api/user_controller.hpp>
#include <copper/components/core.hpp>

namespace copper::components::controllers::api {
void user_controller::invoke(const shared<core>& core,
                             const shared<controller_parameters> parameters,
                             const callback_of<res> on_success,
                             const callback_of<std::exception_ptr> on_error) {
  LOG("[user_controller@invoke] scope_in");
  try {
    const auto user_id = parameters->get_auth().value().id_;

    core->database_->get_user_by_id(
        core, user_id,
        [this, core, parameters, user_id,
         on_success](optional_of<shared<user>> user_opt) {
          LOG("[user_controller@invoke] scope_in [database@get_user_by_id]");

          boost::ignore_unused(user_id);

          // LCOV_EXCL_START
          if (!user_opt) {
            const json::object err = {{"message", "User not found."}};
            LOG("[user_controller@invoke] scope_out [2 of 2] "
                "[database@get_user_by_id]");
            return on_success(
                make_response(core, parameters, status_code::unauthorized,
                              serialize(err), "application/json"));
          }
          // LCOV_EXCL_STOP

          const auto& user = user_opt.value();
          const json::object data = {
              {"id", user->id_},
              {"name", user->name_},
              {"email", user->email_},
              {"email_verified_at", user->email_verified_at_},
              {"created_at", user->created_at_},
              {"updated_at", user->updated_at_},
          };

          on_success(make_response(core, parameters, status_code::ok,
                                   serialize(data), "application/json"));

          LOG("[user_controller@invoke] scope_out [1 of 2] "
              "[database@get_user_by_id]");
        });

    // LCOV_EXCL_START
  } catch (...) {
    LOG("[user_controller@invoke] catch");
    on_error(std::current_exception());
  }
  // LCOV_EXCL_STOP

  LOG("[user_controller@invoke] scope_out");
}
}  // namespace copper::components::controllers::api
