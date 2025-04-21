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

#include <copper/components/controllers/api/up_controller.hpp>
#include <copper/components/core.hpp>

namespace copper::components::controllers::api {
void up_controller::invoke(const shared<core>& core,
                           const shared<controller_parameters> parameters,
                           const callback_of<res> on_success,
                           const callback_of<std::exception_ptr> /*on_error*/) {
  auto now = chronos::now();
  const json::object data = {{"timestamp", now}};

  on_success(make_response(core, parameters, status_code::ok, serialize(data),
                           "application/json"));
}
}  // namespace copper::components::controllers::api
